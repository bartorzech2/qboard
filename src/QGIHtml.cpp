#include "QGIHtml.h"
#include "S11nQt.h"
#include "utility.h"
#include "MenuHandlerGeneric.h"
#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QAction>
#include <QKeySequence>
#include <QFont>
struct QGIHtml::Impl
{
	Impl()
	{
		
	}
	~Impl()
	{
		
	}
};
QGIHtml::QGIHtml() :
	QGraphicsTextItem(),
	Serializable("QGIHtml"),
	impl(new Impl)
{
	this->setup();
}
void QGIHtml::setup()
{
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
	this->setOpenExternalLinks(true);
	// These widgets normally don't look right with a transparent background,
	// so we explicitly set it to white. Also, if they have no text and therefore
	// no size, they can become invisible when added to a board, so we'll insert
	// some default text.
	this->setHtml("<html><body style='background-color:white'><p>Double-click to edit.</p></body></html>");
}
QGIHtml::~QGIHtml()
{
	qDebug() << "~QGIHtml()";
	delete impl;
}

void QGIHtml::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev )
{
	if( ev->buttons() & Qt::LeftButton )
	{
		ev->accept();
		this->setTextInteractionFlags( Qt::TextEditorInteraction );
	}
	else
	{
		this->QGraphicsTextItem::mouseDoubleClickEvent(ev);
	}
}


void QGIHtml::focusOutEvent( QFocusEvent * event )
{
	event->accept();
	this->setTextInteractionFlags( Qt::NoTextInteraction );
}

void QGIHtml::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	if( ev->buttons() & Qt::MidButton )
	{
		ev->accept();
		QGIHtmlEditor ed(this);
		ed.exec();
		return;
	}
	if( (ev->buttons() & Qt::LeftButton) )
	{
		ev->accept();
		qreal zV = qboard::nextZLevel(this);
		if( zV > this->zValue() )
		{
			this->setZValue(zV);
		}
	}
	this->QGraphicsTextItem::mousePressEvent(ev);
}
bool QGIHtml::serialize( S11nNode & dest ) const
{
	if( ! this->Serializable::serialize( dest ) ) return false;
	typedef S11nNodeTraits NT;
	if( this->metaObject()->propertyCount() )
	{
		S11nNode & pr( s11n::create_child( dest, "properties" ) );
		if( ! QObjectProperties_s11n()( pr, *this ) ) return false;
	}
	return s11n::serialize_subnode( dest, "pos", this->pos() )
		&& s11n::serialize_subnode( dest, "html", this->toHtml() );
}

bool QGIHtml::deserialize(  S11nNode const & src )
{
	if( ! this->Serializable::deserialize( src ) ) return false;
	typedef S11nNodeTraits NT;
	QPointF p;
	if( ! s11n::deserialize_subnode( src, "pos", p ) ) return false;
	this->setPos( p );
	QString html;
	if( ! s11n::deserialize_subnode( src, "html", html ) ) return false;
	this->setHtml(html);
	S11nNode const * ch = s11n::find_child_by_name(src, "properties");
	return ch
		? QObjectProperties_s11n()( *ch, *this )
		: true;
}

void QGIHtml::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
	MenuHandlerQGIHtml mh;
	mh.doMenu( this, event );
}



QGIHtmlEditor::QGIHtmlEditor(QGIHtml * item, QWidget * parent) 
	: QDialog(parent),
	Ui::QGIHtmlEditor(),
	m_item(item)
{
	this->setupUi(this);
	this->textEdit->setPlainText( item->toHtml() );
}
#include <QFont>
void QGIHtmlEditor::textBold(bool b)
{
	QFont fn( this->m_item->font() );
	fn.setWeight( b ? QFont::Bold : QFont::Normal);
	m_item->setFont(fn);
}

void QGIHtmlEditor::accept()
{
	m_item->setHtml( this->textEdit->toPlainText() );
	this->QDialog::accept(); 
}

void QGIHtmlEditor::reject()
{
	this->QDialog::reject(); 
}

MenuHandlerQGIHtml::MenuHandlerQGIHtml()
{
	
}

MenuHandlerQGIHtml::~MenuHandlerQGIHtml()
{
}
#include <QDialog>
#include <QGridLayout>
#include <QTextEdit>
#include <QMessageBox>
void MenuHandlerQGIHtml::showHelp()
{
	QDialog dlg;
	QTextEdit * ed = new QTextEdit();
	ed->setReadOnly(true);
	QGridLayout * lay = new QGridLayout();
	lay->setSpacing(2);
	dlg.setLayout(lay);
	lay->addWidget(ed);
	QStringList li;
	li << "<html><body>";
	li << "<strong>Using the QGIHtml widget:</strong><br/><br/>"
		<< "QGIHtml is a text/HTML widget for placing text directly onto a QBoard.<br/>"
		<< "<strong>Tips:</strong>"
		<< "<ul>"
		  << "<li>Right-click to get the context menu.</li>" 
		  << "<li>Double-click to edit it in-line.</li>"
		  << "<li>Double-middle-click to open an editor dialog, where you can edit it as HTML.</li>"
		<< "</ul>"
		<< "Note that this widget supports only a subset of HTML, as described in the API"
		<< "section named 'Supported HTML Subset' in the Qt Assistant documentation."
		<< "Also note that QTextEdit::setHtml() likes to mangle your HTML a bit,"
		<< "so don't be alarmed when it changes between edits."
		;
	li << "</body></html>";
	ed->setHtml( li.join(" ") );
	dlg.exec();
}
void MenuHandlerQGIHtml::doMenu( QGIHtml * pv, QGraphicsSceneContextMenuEvent * ev )
{
	ev->accept();
	MenuHandlerCommon proxy;
	QMenu * m = proxy.createMenu( pv );
	m->addAction(QIcon(":/QBoard/icon/help.png"),"Help...", this, SLOT(showHelp()) );
	m->exec( ev->screenPos() );
	delete m;
}
