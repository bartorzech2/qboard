/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
** Copyright (C) 2008 Stephan Beal.
**
** This file was part of the example classes of the Qt Toolkit.
**
** This copy has been extended by Stephan Beal to use GNU Readline.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


#include <QScriptEngine>
#include <QScriptValue>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtGui/QApplication>
#include <QtCore/QDir>

#include <stdlib.h>

#include <qboard/GameState.h>
#include <qboard/QBoardView.h>
#include <qboard/ScriptQt.h>
#include <qboard/utility.h>
#include "Readline.hpp"

static bool wantsToQuit;

static QScriptValue qtscript_quit(QScriptContext *ctx, QScriptEngine *eng)
{
    Q_UNUSED(ctx);
    wantsToQuit = true;
    return eng->undefinedValue();
}

static void interactive(QScriptEngine *eng)
{
    QScriptValue global = eng->globalObject();
    QScriptValue quitFunction = eng->newFunction(qtscript_quit);
    if (!global.property(QLatin1String("exit")).isValid())
        global.setProperty(QLatin1String("exit"), quitFunction);
    if (!global.property(QLatin1String("quit")).isValid())
        global.setProperty(QLatin1String("quit"), quitFunction);
    wantsToQuit = false;
    readlinecpp::Readline rl("QBoardScript.history");


    const char *qscript_prompt = "qs> ";
    const char *dot_prompt = ".... ";
    const char *prompt = qscript_prompt;

    QString code;

    forever {
	std::string rline;
	bool breakout = false;
	rline = rl.readline( prompt, breakout );
	if( breakout ) break;
	QString line( rline.c_str() );
        code += line;
        code += QLatin1Char('\n');

        if (line.trimmed().isEmpty()) {
            continue;

        } else if (! eng->canEvaluate(code)) {
            prompt = dot_prompt;

        } else {
            QScriptValue result = eng->evaluate(code, QLatin1String("typein"));
            code.clear();
            prompt = qscript_prompt;
	    if( result.isError() )
	    {
		QStringList backtrace = eng->uncaughtExceptionBacktrace();
		fprintf(stderr,"EXCEPTION: %s\n",qPrintable(result.toString()));
		fprintf(stderr, "Backtrace:\n%s\n",
			qPrintable(backtrace.join("\n")));
	    }
            else if (! result.isUndefined())
	    {
                fprintf(stderr, "%s\n", qPrintable(result.toString()));
	    }
            if (wantsToQuit)
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    //QDir::setCurrent( qboard::home().absolutePath() );
    QApplication *app;
    if (argc >= 2 && !qstrcmp(argv[1], "-tty")) {
        ++argv;
       --argc;
        app = new QApplication(argc, argv, QApplication::Tty);
    } else {
        app = new QApplication(argc, argv);
    }

    GameState gstate;
    QScriptEngine *eng = &gstate.jsEngine();

    QStringList paths = app->libraryPaths();
    QDir qhome( qboard::home() );
    paths <<  QString("%1/QBoard/plugins").arg(qhome.absolutePath());
    app->setLibraryPaths(paths);
#if 1
    QScriptValue impv;
#define IMP(M) impv=eng->importExtension(# M); \
    if(impv.isError()) {\
	qDebug() << "Error importing module"<<# M<<":"<<impv.toString(); \
	if(0) return 1;							\
    }
//     IMP(qt.core);
//     IMP(qt.gui);
//     IMP(qt.xml);
//     IMP(qt.svg);
//     IMP(qt.network);
//     IMP(qt.sql);
//     IMP(qt.opengl);
//     IMP(qt.webkit);
//     IMP(qt.xmlpatterns);
//     IMP(qt.uitools);
#undef IMP
#endif

    QScriptValue globalObject = eng->globalObject();
    globalObject.setProperty("__FILE__", QScriptValue(eng,"stdin") );

    if (! *++argv) {
        interactive(eng);
        return EXIT_SUCCESS;
    }

    while (const char *arg = *argv++) {
        QString fn = QString::fromLocal8Bit(arg);

        if (fn == QLatin1String("-i")) {
            interactive(eng);
            break;
        }

        QString contents;
        int lineNumber = 1;

	QScriptValue r;
        if (fn == QLatin1String("-")) {
            QTextStream stream(stdin, QFile::ReadOnly);
            contents = stream.readAll();
	    r = eng->evaluate(contents, fn, lineNumber);
        }
        else {
	    r = qboard::jsInclude( eng, fn );
        }
        if(r.isError()) {
            QStringList backtrace = eng->uncaughtExceptionBacktrace();
            fprintf (stderr, "JS exception:\n    %s\n%s\n\n", qPrintable(r.toString()),
                     qPrintable(backtrace.join("\n")));
            return EXIT_FAILURE;
        }
    }

    delete app;
    return EXIT_SUCCESS;
}
