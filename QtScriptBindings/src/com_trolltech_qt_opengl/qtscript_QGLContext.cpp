#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <qgl.h>
#include <QVariant>
#include <qcolor.h>
#include <qgl.h>
#include <qpaintdevice.h>

#include "qtscriptshell_QGLContext.h"

static const char * const qtscript_QGLContext_function_names[] = {
    "QGLContext"
    // static
    , "currentContext"
    , "setTextureCacheLimit"
    , "textureCacheLimit"
    // prototype
    , "create"
    , "device"
    , "doneCurrent"
    , "format"
    , "isSharing"
    , "isValid"
    , "makeCurrent"
    , "overlayTransparentColor"
    , "requestedFormat"
    , "reset"
    , "setFormat"
    , "swapBuffers"
    , "toString"
};

static const char * const qtscript_QGLContext_function_signatures[] = {
    "QGLFormat format\nQGLFormat format, QPaintDevice device"
    // static
    , ""
    , "int size"
    , ""
    // prototype
    , "QGLContext shareContext"
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , ""
    , "QGLFormat format"
    , ""
""
};

static QScriptValue qtscript_QGLContext_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("QFile::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QGLContext*)
Q_DECLARE_METATYPE(QtScriptShell_QGLContext*)
Q_DECLARE_METATYPE(QPaintDevice*)
Q_DECLARE_METATYPE(QGLFormat)

//
// QGLContext
//

static QScriptValue qtscript_QGLContext_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 12;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    QGLContext* _q_self = qscriptvalue_cast<QGLContext*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("QGLContext.%0(): this object is not a QGLContext")
            .arg(qtscript_QGLContext_function_names[_id+4]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->create();
        return QScriptValue(context->engine(), _q_result);
    }
    if (context->argumentCount() == 1) {
        QGLContext* _q_arg0 = qscriptvalue_cast<QGLContext*>(context->argument(0));
        bool _q_result = _q_self->create(_q_arg0);
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QPaintDevice* _q_result = _q_self->device();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 0) {
        _q_self->doneCurrent();
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        QGLFormat _q_result = _q_self->format();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 4:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isSharing();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 5:
    if (context->argumentCount() == 0) {
        bool _q_result = _q_self->isValid();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    case 6:
    if (context->argumentCount() == 0) {
        _q_self->makeCurrent();
        return context->engine()->undefinedValue();
    }
    break;

    case 7:
    if (context->argumentCount() == 0) {
        QColor _q_result = _q_self->overlayTransparentColor();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 8:
    if (context->argumentCount() == 0) {
        QGLFormat _q_result = _q_self->requestedFormat();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 9:
    if (context->argumentCount() == 0) {
        _q_self->reset();
        return context->engine()->undefinedValue();
    }
    break;

    case 10:
    if (context->argumentCount() == 1) {
        QGLFormat _q_arg0 = qscriptvalue_cast<QGLFormat>(context->argument(0));
        _q_self->setFormat(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 11:
    if (context->argumentCount() == 0) {
        _q_self->swapBuffers();
        return context->engine()->undefinedValue();
    }
    break;

    case 12: {
    QString result = QString::fromLatin1("QGLContext");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_QGLContext_throw_ambiguity_error_helper(context,
        qtscript_QGLContext_function_names[_id+4],
        qtscript_QGLContext_function_signatures[_id+4]);
}

static QScriptValue qtscript_QGLContext_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("QGLContext(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 1) {
        QGLFormat _q_arg0 = qscriptvalue_cast<QGLFormat>(context->argument(0));
        QtScriptShell_QGLContext* _q_cpp_result = new QtScriptShell_QGLContext(_q_arg0);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue((QGLContext*)_q_cpp_result));
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 2) {
        QGLFormat _q_arg0 = qscriptvalue_cast<QGLFormat>(context->argument(0));
        QPaintDevice* _q_arg1 = qscriptvalue_cast<QPaintDevice*>(context->argument(1));
        QtScriptShell_QGLContext* _q_cpp_result = new QtScriptShell_QGLContext(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newVariant(context->thisObject(), qVariantFromValue((QGLContext*)_q_cpp_result));
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    case 1:
    if (context->argumentCount() == 0) {
        QGLContext* _q_result = const_cast<QGLContext*>(QGLContext::currentContext());
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 2:
    if (context->argumentCount() == 1) {
        int _q_arg0 = context->argument(0).toInt32();
        QGLContext::setTextureCacheLimit(_q_arg0);
        return context->engine()->undefinedValue();
    }
    break;

    case 3:
    if (context->argumentCount() == 0) {
        int _q_result = QGLContext::textureCacheLimit();
        return QScriptValue(context->engine(), _q_result);
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_QGLContext_throw_ambiguity_error_helper(context,
        qtscript_QGLContext_function_names[_id],
        qtscript_QGLContext_function_signatures[_id]);
}

QScriptValue qtscript_create_QGLContext_class(QScriptEngine *engine)
{
    static const int function_lengths[] = {
        2
        // static
        , 0
        , 1
        , 0
        // prototype
        , 1
        , 0
        , 0
        , 0
        , 0
        , 0
        , 0
        , 0
        , 0
        , 0
        , 1
        , 0
        , 0
    };
    engine->setDefaultPrototype(qMetaTypeId<QGLContext*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((QGLContext*)0));
    for (int i = 0; i < 13; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QGLContext_prototype_call, function_lengths[i+4]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_QGLContext_function_names[i+4]),
            fun, QScriptValue::SkipInEnumeration);
    }

    engine->setDefaultPrototype(qMetaTypeId<QGLContext*>(), proto);

    QScriptValue ctor = engine->newFunction(qtscript_QGLContext_static_call, proto, function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));
    for (int i = 0; i < 3; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QGLContext_static_call,
            function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i+1)));
        ctor.setProperty(QString::fromLatin1(qtscript_QGLContext_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    return ctor;
}
