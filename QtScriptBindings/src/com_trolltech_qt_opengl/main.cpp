#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include <qgl.h>
#include <qglcolormap.h>
#include <qglpixelbuffer.h>
#include <qglframebufferobject.h>
#include <qgl.h>

QScriptValue qtscript_create_QGLContext_class(QScriptEngine *engine);
QScriptValue qtscript_create_QGLColormap_class(QScriptEngine *engine);
QScriptValue qtscript_create_QGLPixelBuffer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QGLFramebufferObject_class(QScriptEngine *engine);
QScriptValue qtscript_create_QGLWidget_class(QScriptEngine *engine);

static const char * const qtscript_com_trolltech_qt_opengl_class_names[] = {
    "QGLContext"
    , "QGLColormap"
    , "QGLPixelBuffer"
    , "QGLFramebufferObject"
    , "QGLWidget"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator qtscript_com_trolltech_qt_opengl_class_functions[] = {
    qtscript_create_QGLContext_class
    , qtscript_create_QGLColormap_class
    , qtscript_create_QGLPixelBuffer_class
    , qtscript_create_QGLFramebufferObject_class
    , qtscript_create_QGLWidget_class
};

class com_trolltech_qt_opengl_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList com_trolltech_qt_opengl_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qt");
    list << QLatin1String("qt.opengl");
    return list;
}

void com_trolltech_qt_opengl_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qt")) {
    } else if (key == QLatin1String("qt.opengl")) {
        QScriptValue extensionObject = engine->globalObject();
        for (int i = 0; i < 5; ++i) {
            extensionObject.setProperty(qtscript_com_trolltech_qt_opengl_class_names[i],
                qtscript_com_trolltech_qt_opengl_class_functions[i](engine),
                QScriptValue::SkipInEnumeration);
        }
    } else {
        Q_ASSERT_X(false, "com_trolltech_qt_opengl::initialize", qPrintable(key));
    }
}
Q_EXPORT_STATIC_PLUGIN(com_trolltech_qt_opengl_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_com_trolltech_qt_opengl, com_trolltech_qt_opengl_ScriptPlugin)

