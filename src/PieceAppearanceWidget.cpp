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
#include "QGIGamePiece.h"

struct PieceAppearanceWidget::Impl
{
    GameState gs;
    QGraphicsView * gv;
    GamePiece * pc;
    Impl() : gs(), gv(0),pc(0)
    {
    }
    ~Impl()
    {
    }
};

void PieceAppearanceWidget::setupDefaultTemplates()
{
    typedef QList<QColor> QCL;
    QCL cl;
    cl << QColor(255,0,0)
       << QColor(0,255,0)
       << QColor(0,0,255)
       << QColor(255,255,0)
       << QColor(255,255,255)

       << QColor(255,127,127)
       << QColor(127,255,127)
       << QColor(127,127,255)
       << QColor(127,127,0)
       << QColor(127,127,127)

       << QColor(127,0,0)
       << QColor(0,127,0)
       << QColor(0,0,127)
       << QColor(64,64,0)
       << QColor(0,0,0,0)

	;
    int step = 24;
    int space = 2;
    int x = space;
    int y = space;
    int count = cl.size();
    int pos = 0;
    int rows = 3;
    QGraphicsItem * qgi = 0;
    for( QCL::iterator it = cl.begin();
	 cl.end() != it; ++it )
    {
	GamePiece * pc = new GamePiece;
	if( ! this->impl->pc ) this->impl->pc = pc;
	qgi = this->impl->gs.addPiece( pc );
	pc->setPieceProperty("size",QSize(step,step));
	pc->setPieceProperty("pos",QPoint(x,y));
	pc->setPieceProperty("dragDisabled",int(1));
	//x += step + space;
	x += step + space;
	if( ++pos >= (count/rows) )
	{
	    y += step + space;
	    pos = 0;
	    x = space;
	}
	pc->setPieceProperty("color",*it);
	pc->setPieceProperty("borderSize",1);
	pc->setPieceProperty("borderColor",QColor(0,0,0));
    }

}
void PieceAppearanceWidget::setupUI()
{
    QLayout * lay = new QGridLayout( this );
    lay->setSpacing(0);
    lay->setContentsMargins(0,0,0,0);
    QGraphicsView * v = this->impl->gv = new QGraphicsView( impl->gs.scene() );
	//new QBoardView(impl->board(), impl->scene());

    v->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    v->setInteractive(true); // required to get mouse events to the children
    v->setTransformationAnchor(QGraphicsView::NoAnchor);
    //v->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    v->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    v->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    //v->setDragMode(QGraphicsView::ScrollHandDrag);
    v->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    v->setBackgroundBrush(QColor("#abb8fb"));
    v->viewport()->setObjectName( "PieceAppearanceWidget");

    lay->addWidget( v );

    QStringList tip;
    tip << "<html><body>These widgets are appearance templates for game pieces.\n"
	<< "Edit them by right-clicking them.\n"
	<< "Select a template by left-clicking it. "
	<< "QBoard will use the selected template as the default look\n"
	<< "for newly-added pieces which are loaded from image files.\n"
	<< "This list will be saved automatically when QBoard exits,\n"
	<< "and loaded when QBoard starts up.</body></html>"
	;
    impl->gv->setToolTip( tip.join("") );

}
PieceAppearanceWidget::PieceAppearanceWidget(QWidget * parent, Qt::WindowFlags flags) 
    : QWidget(parent,flags), Serializable("PieceAppearanceWidget"),
      impl(new Impl)
{
    this->setupUI();
}

PieceAppearanceWidget::~PieceAppearanceWidget()
{
    //qDebug() << "~PieceAppearanceWidget()";
    delete impl;
}

QGraphicsView * PieceAppearanceWidget::view()
{
    return this->impl->gv;
}
#include <QDynamicPropertyChangeEvent>
bool PieceAppearanceWidget::event( QEvent * e )
{
	return QObject::event(e);
}

GameState & PieceAppearanceWidget::state()
{
    return this->impl->gs;
}
GameState const & PieceAppearanceWidget::state() const
{
    return this->impl->gs;
}


bool PieceAppearanceWidget::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    //typedef S11nNodeTraits NT;
    return s11n::serialize_subnode( dest, "gamestate", this->impl->gs );
}

bool PieceAppearanceWidget::deserialize(  S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    //typedef S11nNodeTraits NT;
    return s11n::deserialize_subnode( src, "gamestate", this->impl->gs );
}
void PieceAppearanceWidget::clear()
{
    this->impl->gs.clear();
}

void PieceAppearanceWidget::applyCurrentTemplate( GamePiece * tgt )
{
    if( !tgt ) return;
    typedef QList<QGraphicsItem*> QIL;
    QIL li( impl->gv->scene()->selectedItems() );
    QGIGamePiece * pvi = 0;
    impl->pc = 0;
    // If there are multiple items selected, simply pick the first
    // selected item which has an associated piece (the order has no
    // meaning, however).
    for( QIL::iterator it = li.begin();
	 li.end() != it;
	 ++it )
    {
	pvi = dynamic_cast<QGIGamePiece*>(*it);
	if( pvi )
	{
	    impl->pc = pvi->piece();
	    if( impl->pc ) break;
	}
    }
    if( ! impl->pc ) return;

    QVariant pix( tgt->property("pixmap") );
    QVariant pos( tgt->property("pos") );
    QVariant dragDisabled( tgt->property("dragDisabled") );
    {
	S11nNode n;
	impl->pc->serialize( n );
	tgt->deserialize( n );
    }
    tgt->setPieceProperty("pos", pos );
    tgt->setPieceProperty("pixmap", pix );
    tgt->setPieceProperty("dragDisabled", dragDisabled );
}
