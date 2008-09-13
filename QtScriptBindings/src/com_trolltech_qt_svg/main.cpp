#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include <qsvgrenderer.h>
#include <qsvggenerator.h>
#include <qsvgwidget.h>

QScriptValue qtscript_create_QSvgRenderer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSvgGenerator_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSvgWidget_class(QScriptEngine *engine);

static const char * const qtscript_com_trolltech_qt_svg_class_names[] = {
    "QSvgRenderer"
    , "QSvgGenerator"
    , "QSvgWidget"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator qtscript_com_trolltech_qt_svg_class_functions[] = {
    qtscript_create_QSvgRenderer_class
    , qtscript_create_QSvgGenerator_class
    , qtscript_create_QSvgWidget_class
};

class com_trolltech_qt_svg_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList com_trolltech_qt_svg_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qt");
    list << QLatin1String("qt.svg");
    return list;
}

void com_trolltech_qt_svg_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qt")) {
    } else if (key == QLatin1String("qt.svg")) {
        QScriptValue extensionObject = engine->globalObject();
        for (int i = 0; i < 3; ++i) {
            extensionObject.setProperty(qtscript_com_trolltech_qt_svg_class_names[i],
                qtscript_com_trolltech_qt_svg_class_functions[i](engine),
                QScriptValue::SkipInEnumeration);
        }
    } else {
        Q_ASSERT_X(false, "com_trolltech_qt_svg::initialize", qPrintable(key));
    }
}
Q_EXPORT_STATIC_PLUGIN(com_trolltech_qt_svg_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_com_trolltech_qt_svg, com_trolltech_qt_svg_ScriptPlugin)

