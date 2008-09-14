#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include "Readline.hpp"

static readlinecpp::Readline & reader()
{
    static readlinecpp::Readline bob;
    return bob;
}


class readline_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList readline_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("readline");
    return list;
}

static QScriptValue js_readline(QScriptContext *ctx, QScriptEngine *eng)
{
    QString prompt = ctx->argumentCount()
	? ctx->argument(0).toString()
	: QString(" >");
    bool breakout = false;
    std::string str( reader().readline( prompt.toLatin1().constData(), breakout ) );
    return breakout
	? eng->undefinedValue()
	: QScriptValue( eng, str.empty()
			? QString()
			: QString(str.c_str()) );
}

static QScriptValue js_loadHistory(QScriptContext *ctx, QScriptEngine *eng)
{
    return ctx->argumentCount()
	? QScriptValue( eng,
			reader().load_history( ctx->argument(0).toString().toLatin1().constData() ) )
	: eng->undefinedValue();
}
static QScriptValue js_saveHistory(QScriptContext *ctx, QScriptEngine *eng)
{
    return ctx->argumentCount()
	? QScriptValue( eng,
			reader().save_history( ctx->argument(0).toString().toLatin1().constData() ) )
	: eng->undefinedValue();
}
static QScriptValue js_clearHistory(QScriptContext *, QScriptEngine *eng)
{
    reader().clear_history();
    return eng->undefinedValue();
}

void readline_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key != QLatin1String("readline"))
    {
        Q_ASSERT_X(false, "readline_ScriptPlugin::initialize", qPrintable(key));
	return;
    }
    QScriptValue glob = engine->globalObject();
    QScriptValue rl( engine->newObject() );
    glob.setProperty("Readline", rl );
    rl.setProperty( "read", engine->newFunction(js_readline) );
    QScriptValue hi( engine->newObject() );
    rl.setProperty("history", hi );
    hi.setProperty( "load", engine->newFunction(js_loadHistory) );
    hi.setProperty( "save", engine->newFunction(js_saveHistory) );
    hi.setProperty( "clear", engine->newFunction(js_clearHistory) );
}
Q_EXPORT_STATIC_PLUGIN(readline_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_readline, readline_ScriptPlugin)

