#ifndef QGIHTMLL_H
#define QGIHTMLL_H
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


#include <QGraphicsTextItem>

#include "Serializable.h"
#include "QGI.h"

class QGraphicsSceneContextMenuEvent;
class QEvent;
/**
   QGIHtml implements a serializale QGraphicsTextItem which acts
   as an on-board text/html widget.
*/
class QGIHtml : public QGraphicsTextItem, public Serializable
{
Q_OBJECT
public:
    QGIHtml();
    virtual ~QGIHtml();
    /** Serializes this object to dest. The following properties are saved:
	position, text (as HTML), QObject "dynamic" properties.
    */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );
    virtual int type() const { return QGITypes::Html; }
protected:
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event );
    virtual void focusOutEvent( QFocusEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual bool event( QEvent * e );
private:
    void refreshTransformation();
    void setup();
    struct Impl;
    Impl * impl;
};

// Register QGIHtml with s11n:
#define S11N_TYPE QGIHtml
#define S11N_TYPE_NAME "QGIHtml"
#define S11N_BASE_TYPE Serializable
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE QGIHtml
#define S11N_TYPE_NAME "QGIHtml"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>

#include <QDialog>
#include "ui_QGIHtmlEditor.h"
class QGIHtmlEditor : public QDialog, public Ui::QGIHtmlEditor
{
Q_OBJECT
public:
    QGIHtmlEditor( QGIHtml * item, QWidget * parent = 0 );
public Q_SLOTS:
    virtual void accept();
    virtual void reject();
private Q_SLOTS:
    void textBold(bool);
private:
    QGIHtml * m_item;
};

/*
  Context menu handler for QGIHtml object.
*/
class QGraphicsSceneContextMenuEvent;
class MenuHandlerQGIHtml : public QObject
{
    Q_OBJECT
public:
    MenuHandlerQGIHtml();
    virtual ~MenuHandlerQGIHtml();
public Q_SLOTS:
    void doMenu( QGIHtml *, QGraphicsSceneContextMenuEvent * );
private Q_SLOTS:
    void copyList();
    void cutList();
    void showHelp();
private:
    static void clipList( QGIHtml * src, bool copy );
    QGIHtml * m_gi;
};

#endif
