#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptValue>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <qmetaobject.h>

#include <qabstractpagesetupdialog.h>
#include <QVariant>
#include <qaction.h>
#include <qbitmap.h>
#include <qbytearray.h>
#include <qcoreevent.h>
#include <qcursor.h>
#include <qdialog.h>
#include <qevent.h>
#include <qfont.h>
#include <qicon.h>
#include <qinputcontext.h>
#include <qkeysequence.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlocale.h>
#include <qobject.h>
#include <qpaintdevice.h>
#include <qpaintengine.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpoint.h>
#include <qprinter.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstyle.h>
#include <qwidget.h>

#include "qtscriptshell_QAbstractPageSetupDialog.h"

static const char * const qtscript_QAbstractPageSetupDialog_function_names[] = {
    "QAbstractPageSetupDialog"
    // static
    // prototype
    , "printer"
    , "toString"
};

static const char * const qtscript_QAbstractPageSetupDialog_function_signatures[] = {
    "QPrinter printer, QWidget parent"
    // static
    // prototype
    , ""
""
};

static QScriptValue qtscript_QAbstractPageSetupDialog_throw_ambiguity_error_helper(
    QScriptContext *context, const char *functionName, const char *signatures)
{
    QStringList lines = QString::fromLatin1(signatures).split(QLatin1Char('\n'));
    QStringList fullSignatures;
    for (int i = 0; i < lines.size(); ++i)
        fullSignatures.append(QString::fromLatin1("%0(%1)").arg(functionName).arg(lines.at(i)));
    return context->throwError(QString::fromLatin1("QAbstractPageSetupDialog::%0(): could not find a function match; candidates are:\n%1")
        .arg(functionName).arg(fullSignatures.join(QLatin1String("\n"))));
}

Q_DECLARE_METATYPE(QAbstractPageSetupDialog*)
Q_DECLARE_METATYPE(QtScriptShell_QAbstractPageSetupDialog*)
Q_DECLARE_METATYPE(QPrinter*)
Q_DECLARE_METATYPE(QDialog*)

//
// QAbstractPageSetupDialog
//

static QScriptValue qtscript_QAbstractPageSetupDialog_prototype_call(QScriptContext *context, QScriptEngine *)
{
#if QT_VERSION > 0x040400
    Q_ASSERT(context->callee().isFunction());
    uint _id = context->callee().data().toUInt32();
#else
    uint _id;
    if (context->callee().isFunction())
        _id = context->callee().data().toUInt32();
    else
        _id = 0xBABE0000 + 1;
#endif
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    QAbstractPageSetupDialog* _q_self = qscriptvalue_cast<QAbstractPageSetupDialog*>(context->thisObject());
    if (!_q_self) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("QAbstractPageSetupDialog.%0(): this object is not a QAbstractPageSetupDialog")
            .arg(qtscript_QAbstractPageSetupDialog_function_names[_id+1]));
    }

    switch (_id) {
    case 0:
    if (context->argumentCount() == 0) {
        QPrinter* _q_result = _q_self->printer();
        return qScriptValueFromValue(context->engine(), _q_result);
    }
    break;

    case 1: {
    QString result = QString::fromLatin1("QAbstractPageSetupDialog");
    return QScriptValue(context->engine(), result);
    }

    default:
    Q_ASSERT(false);
    }
    return qtscript_QAbstractPageSetupDialog_throw_ambiguity_error_helper(context,
        qtscript_QAbstractPageSetupDialog_function_names[_id+1],
        qtscript_QAbstractPageSetupDialog_function_signatures[_id+1]);
}

static QScriptValue qtscript_QAbstractPageSetupDialog_static_call(QScriptContext *context, QScriptEngine *)
{
    uint _id = context->callee().data().toUInt32();
    Q_ASSERT((_id & 0xFFFF0000) == 0xBABE0000);
    _id &= 0x0000FFFF;
    switch (_id) {
    case 0:
    if (context->thisObject().strictlyEquals(context->engine()->globalObject())) {
        return context->throwError(QString::fromLatin1("QAbstractPageSetupDialog(): Did you forget to construct with 'new'?"));
    }
    if (context->argumentCount() == 1) {
        QPrinter* _q_arg0 = qscriptvalue_cast<QPrinter*>(context->argument(0));
        QtScriptShell_QAbstractPageSetupDialog* _q_cpp_result = new QtScriptShell_QAbstractPageSetupDialog(_q_arg0);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (QAbstractPageSetupDialog*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    } else if (context->argumentCount() == 2) {
        QPrinter* _q_arg0 = qscriptvalue_cast<QPrinter*>(context->argument(0));
        QWidget* _q_arg1 = qscriptvalue_cast<QWidget*>(context->argument(1));
        QtScriptShell_QAbstractPageSetupDialog* _q_cpp_result = new QtScriptShell_QAbstractPageSetupDialog(_q_arg0, _q_arg1);
        QScriptValue _q_result = context->engine()->newQObject(context->thisObject(), (QAbstractPageSetupDialog*)_q_cpp_result, QScriptEngine::AutoOwnership);
        _q_cpp_result->__qtscript_self = _q_result;
        return _q_result;
    }
    break;

    default:
    Q_ASSERT(false);
    }
    return qtscript_QAbstractPageSetupDialog_throw_ambiguity_error_helper(context,
        qtscript_QAbstractPageSetupDialog_function_names[_id],
        qtscript_QAbstractPageSetupDialog_function_signatures[_id]);
}

static QScriptValue qtscript_QAbstractPageSetupDialog_toScriptValue(QScriptEngine *engine, QAbstractPageSetupDialog* const &in)
{
    return engine->newQObject(in, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

static void qtscript_QAbstractPageSetupDialog_fromScriptValue(const QScriptValue &value, QAbstractPageSetupDialog* &out)
{
    out = qobject_cast<QAbstractPageSetupDialog*>(value.toQObject());
}

QScriptValue qtscript_create_QAbstractPageSetupDialog_class(QScriptEngine *engine)
{
    static const int function_lengths[] = {
        2
        // static
        // prototype
        , 0
        , 0
    };
    engine->setDefaultPrototype(qMetaTypeId<QAbstractPageSetupDialog*>(), QScriptValue());
    QScriptValue proto = engine->newVariant(qVariantFromValue((QAbstractPageSetupDialog*)0));
    proto.setPrototype(engine->defaultPrototype(qMetaTypeId<QDialog*>()));
    for (int i = 0; i < 2; ++i) {
        QScriptValue fun = engine->newFunction(qtscript_QAbstractPageSetupDialog_prototype_call, function_lengths[i+1]);
        fun.setData(QScriptValue(engine, uint(0xBABE0000 + i)));
        proto.setProperty(QString::fromLatin1(qtscript_QAbstractPageSetupDialog_function_names[i+1]),
            fun, QScriptValue::SkipInEnumeration);
    }

    qScriptRegisterMetaType<QAbstractPageSetupDialog*>(engine, qtscript_QAbstractPageSetupDialog_toScriptValue, 
        qtscript_QAbstractPageSetupDialog_fromScriptValue, proto);

    QScriptValue ctor = engine->newFunction(qtscript_QAbstractPageSetupDialog_static_call, proto, function_lengths[0]);
    ctor.setData(QScriptValue(engine, uint(0xBABE0000 + 0)));

    return ctor;
}
