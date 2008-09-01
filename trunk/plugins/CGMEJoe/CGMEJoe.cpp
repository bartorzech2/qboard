#include <QtGui>
#include <QLabel>
#include <QDebug>
#include <QTextEdit>
#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QScriptEngine>
#include <QScriptValue>
#include <QMessageBox>

#include <stdexcept>
#include "CGMEJoe.h"
#include "GameState.h"

struct CGMEJoe::Impl
{
    QWidget * wid;
    QTextEdit * editor;
    GameState * gs;
    Impl() :
	wid(0),
	editor(0),
	gs(0)
    {
    }
    ~Impl()
    {
	delete wid;
	wid = 0;
    }
};

CGMEJoe::CGMEJoe() : impl(new Impl)
{
    qDebug() << "CGMEJoe()";
}

CGMEJoe::~CGMEJoe()
{
    qDebug() << "~CGMEJoe()";
    if( impl->wid )
    {
	QObject::disconnect( impl->wid, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()) );
    }
    delete impl;
}

void CGMEJoe::widgetDestroyed()
{
    impl->wid = impl->editor = 0;
}


void CGMEJoe::evalJS()
{
    if( ! impl->editor ) return;
    QString code( impl->editor->toPlainText() );
    if( code.isEmpty() ) return;
    try
    {
	QScriptEngine & js( impl->gs->jsEngine() );
	QScriptValue rv = js.evaluate(code, "CGMEJoe editor");
#if QT_VERSION >= 0x040400
	if( rv.isError() )
#else
	if( js.hasUncaughtException() ) // in Qt 4.4.1 this fails!
#endif
	{
	    QStringList bt( js.uncaughtExceptionBacktrace() );
	    QScriptValue exv = rv.isError() ? rv : js.uncaughtException();
	    QString msg("Script threw or returned an exception:\n");
	    msg += exv.toString()
		+ "\nBacktrace:\n"
		+ bt.join("\n");
	    QMessageBox::warning( impl->editor,
				  "JavaScript Exception",
				  msg,
				  QMessageBox::Ok,
				  QMessageBox::Ok );
	}
    }
    catch(std::exception const & ex)
    {
	qDebug() << "CGMEJoe::evalJS() got a native exception:" << ex.what();
    }
}

QWidget * CGMEJoe::widget()
{
    if( ! impl->wid )
    {
	QFrame * fr = new QFrame;
	QGridLayout * lay = new QGridLayout(fr);
	QPushButton * but = new QPushButton("Evaluate");
	QTextEdit * editor = new QTextEdit;
	editor->setPlainText("qboard.createObject('QGIHtml',{pos:qboard.placementPos()});");
	lay->addWidget( editor, 0, 0 );
	lay->addWidget( but, 1, 0);
	connect( but, SIGNAL(clicked()), this,SLOT(evalJS()));
	connect( fr, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()) );
	impl->wid = fr;
	impl->editor = editor;
    }
    return impl->wid;
}


void CGMEJoe::setGameState(GameState &s)
{
    impl->gs = &s;
}

Q_EXPORT_PLUGIN2(CGMEJoe, CGMEJoe)
