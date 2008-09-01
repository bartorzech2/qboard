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
#include "JSEditor.h"
#include "GameState.h"
#include "highlighter.h"

struct JSEditor::Impl
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

JSEditor::JSEditor() : impl(new Impl)
{
    qDebug() << "JSEditor()";
}

JSEditor::~JSEditor()
{
    qDebug() << "~JSEditor()";
    if( impl->wid )
    {
	QObject::disconnect( impl->wid, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()) );
    }
    delete impl;
}

void JSEditor::widgetDestroyed()
{
    impl->wid = impl->editor = 0;
}


void JSEditor::evalJS()
{
    if( ! impl->editor ) return;
    QString code( impl->editor->toPlainText() );
    if( code.isEmpty() ) return;
    try
    {
	QScriptEngine & js( impl->gs->jsEngine() );
	QScriptValue rv = js.evaluate(code, "JSEditor editor");
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
	qDebug() << "JSEditor::evalJS() got a native exception:" << ex.what();
    }
}

QWidget * JSEditor::widget()
{
    if( ! impl->wid )
    {
	QString srccode = "qboard.createObject('QGIHtml',{pos:qboard.placementPos()});";
	QFrame * fr = new QFrame;
	QGridLayout * lay = new QGridLayout(fr);
	QPushButton * but = new QPushButton("Evaluate");
	QTextEdit * editor = new QTextEdit;
	Highlighter * h = new Highlighter(editor->document());
	editor->setPlainText(srccode);
	lay->addWidget( editor, 0, 0 );
	lay->addWidget( but, 1, 0);
	connect( but, SIGNAL(clicked()), this,SLOT(evalJS()));
	connect( fr, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()) );
	impl->wid = fr;
	impl->editor = editor;
    }
    return impl->wid;
}


void JSEditor::setGameState(GameState &s)
{
    impl->gs = &s;
}

Q_EXPORT_PLUGIN2(JSEditor, JSEditor)
