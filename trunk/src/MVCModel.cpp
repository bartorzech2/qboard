#include "MVCModel.h"

#include <QList>
#include <QByteArray>
#include <QVariant>
#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>
#include <QPalette>
#include <QEvent>
#include <QDynamicPropertyChangeEvent>

#include <s11n.net/s11n/s11n.hpp>
#include <s11n.net/s11n/functional.hpp>
#include <iterator>
#include <algorithm>
#include <memory>

#include "utility.h"
#include "S11nQt.h"

struct MVCModel::Impl
{
    bool destructing;
    int viewCount;
    Impl() :
	destructing(false),
	viewCount(0)
    {}
    ~Impl(){}
};

MVCModel::MVCModel() :
    QObject(),
    Serializable("MVCModel"),
    impl(new Impl)
{
    this->s11nFileExtension(".MVCModel");
}

MVCModel::~MVCModel() 
{
    impl->destructing = true;
    QBOARD_VERBOSE_DTOR << "~MVCModel()" << this;
    emit destructing(this);
    emit destroyed(); // we do this here to ensure that the proper polymorphic type is available when destroyed() is fired.
    delete impl;
}

bool MVCModel::hasProperty( char const * key )
{
    return this->property(key).isValid();
}

bool MVCModel::setModelProperty( char const * name, QVariant const & v)
{
    if( impl->destructing ) return false;
    if( ! QVariant_s11n::canHandle( v.type() ) )
    {
	//qDebug() << "MVCModel::setModelProperty(["<<name<<"],["<<v<<"]) rejecting unknown QVariant type.";
	return false;
    }
    return this->setProperty(name,v); // see event(): that triggers a propertySet() signal
}

bool MVCModel::event( QEvent * e )
{
    if( impl->destructing ) return false;
    if( e->type() == QEvent::DynamicPropertyChange )
    {
	e->accept();
	QDynamicPropertyChangeEvent *chev = dynamic_cast<QDynamicPropertyChangeEvent *>(e);
	if( ! chev ) return false; 
	emit propertySet( chev->propertyName().constData(), this );
	return true;
    }
    return QObject::event(e);
}

void MVCModel::addViewRef()
{
    ++(impl->viewCount); 
}
void MVCModel::removeViewRef()
{
    if( (!impl->destructing) && (0 == --(impl->viewCount)))
    {
	this->deleteLater();
    }
}

bool MVCModel::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    S11nNode & ch( s11n::create_child(dest, "properties"));
    S11nNodeTraits::class_name(ch,"QList");
    return QObjectProperties_s11n()( ch, *this ); 
}

bool MVCModel::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    // FIXME: we need to unset ALL properties before we do this.
    // We can do that via setProperty(foo,QVariant()).
    S11nNode const * ch = s11n::find_child_by_name(src, "properties");
    if( ! ch ) return false;
    return QObjectProperties_s11n()( *ch, *this );
}



MVCManagedList::MVCManagedList() : Serializable("MVCManagedList")
{
    this->s11nFileExtension(".QBoardMVCManagedList");
}
MVCManagedList::~MVCManagedList()
{
    QBOARD_VERBOSE_DTOR << "~MVCManagedList()";
    this->clearModels();
}

void MVCManagedList::connect( MVCModel * gp )
{
    this->QObject::connect(gp,SIGNAL(destructing(MVCModel *)),this,SLOT(removeModel(MVCModel*)));
}
void MVCManagedList::disconnect( MVCModel * gp )
{
    QObject::disconnect(gp,SIGNAL(destructing(MVCModel *)),this,SLOT(removeModel(MVCModel*)));
}

void MVCManagedList::clearModels()
{
    iterator it = this->begin();
    for( ; it != this->end(); ++it )
    {
	this->disconnect(*it);
	emit objectRemoved(*it);
	delete (*it);
    }
    this->m_list.clear();
}

void MVCManagedList::clearNoDelete()
{
    this->m_list.clear();
}

void MVCManagedList::addModel( MVCModel * gp )
{
    this->connect(gp);
    this->m_list.insert( gp );
    emit objectAdded(gp);
}

bool MVCManagedList::removeModel( MVCModel * gp )
{
    iterator it =this->m_list.find(gp);
    if( m_list.end() == it ) return false;
    this->disconnect(gp); 
    this->m_list.erase(it);
    emit objectRemoved(gp);
    return true;
}

void MVCManagedList::takeObjects( MVCManagedList &other )
{
    iterator it = other.m_list.begin();
    iterator et = other.m_list.end();
    for( ; et != it; ++it )
    {
	other.removeModel(*it);
	this->addModel(*it);
    }
}

bool MVCManagedList::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    std::for_each( this->m_list.begin(), this->m_list.end(),
		   s11n::ser_to_subnode_unary_f( dest, "object" ) );
    return true; 
}

bool MVCManagedList::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    this->clearModels();
    typedef S11nNodeTraits NT;
    NT::child_list_type::const_iterator it = NT::children(src).begin();
    NT::child_list_type::const_iterator et = NT::children(src).end();
    typedef s11n::cleanup_ptr<MVCModel> GP;
    for( ; et != it; ++it )
    {
	qDebug() <<"MVCManagedList::deserialize() doing child...";
	GP gp( s11nlite::deserialize<MVCModel>( *(*it) ) );
	if( ! gp.get() )
	{
	    return false;
	}
	this->addModel(gp.release());
    }
    return true;
}
