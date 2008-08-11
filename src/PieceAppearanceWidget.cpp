/*
 * This file is (or was, at some point) part of the QBoard project
 * (http://code.google.com/p/qboard)
 *
 * Copyright (c) 2008 Stephan Beal (http://wanderinghorse.net/home/stephan/)
 *
 * This file may be used under the terms of the GNU General Public
 * License versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 * included in the packaging of this file.
 *
 */

#include "PieceAppearanceWidget.h"

#include <s11n.net/s11n/s11nlite.hpp>
#include <QDebug>
#include <QGridLayout>
#include <QGraphicsView>
#include "S11nQt.h"
#include "QBoardView.h"
void PieceAppearanceWidget::setupUI()
{
    QLayout * lay = new QGridLayout( this );
    lay->setSpacing(0);
    lay->setContentsMargins(2,2,2,2);
    QGraphicsView * v = this->m_gv = new QGraphicsView( m_gs.scene() );
	//new QBoardView(m_gs.board(), m_gs.scene());

    v->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    v->setInteractive(true); // required to get mouse events to the children
    v->setTransformationAnchor(QGraphicsView::NoAnchor);
    //v->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    v->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    v->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    //v->setDragMode(QGraphicsView::RubberBandDrag);
    v->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    v->setBackgroundBrush(QColor("#abb8fb"));
    v->viewport()->setObjectName( "PieceAppearanceWidget");

    lay->addWidget( v );
    this->setBackgroundRole( QPalette::HighlightedText );

}
PieceAppearanceWidget::PieceAppearanceWidget(QWidget * parent, Qt::WindowFlags flags) 
    : QWidget(parent,flags), Serializable("PieceAppearanceWidget"),
      m_gs(),
      m_gv(0)
{
    this->setupUI();
}

PieceAppearanceWidget::~PieceAppearanceWidget()
{
    //qDebug() << "~PieceAppearanceWidget()";
}

QGraphicsView * PieceAppearanceWidget::view()
{
    return this->m_gv;
}
#include <QDynamicPropertyChangeEvent>
bool PieceAppearanceWidget::event( QEvent * e )
{
#if 0
	if( e->type() == QEvent::DynamicPropertyChange )
	{
	    e->accept();
	    QDynamicPropertyChangeEvent * dev = dynamic_cast<QDynamicPropertyChangeEvent*>(e);
	    QString key(dev->propertyName());
	    if( QString("pixmap") == key )
	    {
		QVariant var( this->property("pixmap") );
		if( var.isValid() )
		{
		    this->load( var.toString() );
		}
		else
		{
		    this->m_file = QString();
		    this->m_px = QPixmap();
		    emit loaded();
		}
	    }
	    return true;
	}
#endif
	return QObject::event(e);
}

GameState & PieceAppearanceWidget::state()
{
    return this->m_gs;
}
GameState const & PieceAppearanceWidget::state() const
{
    return this->m_gs;
}


bool PieceAppearanceWidget::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    //return s11n::serialize_subnode( dest, "pixmap", this->m_file );
    //&& s11n::serialize_subnode( dest, "size", this->m_px.size() );
    return true;
}

bool PieceAppearanceWidget::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    return true;
}
void PieceAppearanceWidget::clear()
{
    this->m_gs.clear();
}

