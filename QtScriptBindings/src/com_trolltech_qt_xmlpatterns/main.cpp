#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include <qxmlquery.h>
#include <qxmlnamepool.h>
#include <qxmlresultitems.h>
#include <qabstractxmlnodemodel.h>
#include <qabstractxmlnodemodel.h>
#include <qxmlname.h>
#include <qsourcelocation.h>
#include <qabstractxmlnodemodel.h>
#include <qabstractxmlreceiver.h>
#include <qabstractmessagehandler.h>
#include <qabstracturiresolver.h>
#include <qxmlserializer.h>
#include <qsimplexmlnodemodel.h>
#include <qxmlformatter.h>

QScriptValue qtscript_create_QXmlQuery_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlNamePool_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlResultItems_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlNodeModelIndex_class(QScriptEngine *engine);
QScriptValue qtscript_create_QAbstractXmlNodeModel_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlName_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSourceLocation_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlItem_class(QScriptEngine *engine);
QScriptValue qtscript_create_QAbstractXmlReceiver_class(QScriptEngine *engine);
QScriptValue qtscript_create_QAbstractMessageHandler_class(QScriptEngine *engine);
QScriptValue qtscript_create_QAbstractUriResolver_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlSerializer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSimpleXmlNodeModel_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlFormatter_class(QScriptEngine *engine);

static const char * const qtscript_com_trolltech_qt_xmlpatterns_class_names[] = {
    "QXmlQuery"
    , "QXmlNamePool"
    , "QXmlResultItems"
    , "QXmlNodeModelIndex"
    , "QAbstractXmlNodeModel"
    , "QXmlName"
    , "QSourceLocation"
    , "QXmlItem"
    , "QAbstractXmlReceiver"
    , "QAbstractMessageHandler"
    , "QAbstractUriResolver"
    , "QXmlSerializer"
    , "QSimpleXmlNodeModel"
    , "QXmlFormatter"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator qtscript_com_trolltech_qt_xmlpatterns_class_functions[] = {
    qtscript_create_QXmlQuery_class
    , qtscript_create_QXmlNamePool_class
    , qtscript_create_QXmlResultItems_class
    , qtscript_create_QXmlNodeModelIndex_class
    , qtscript_create_QAbstractXmlNodeModel_class
    , qtscript_create_QXmlName_class
    , qtscript_create_QSourceLocation_class
    , qtscript_create_QXmlItem_class
    , qtscript_create_QAbstractXmlReceiver_class
    , qtscript_create_QAbstractMessageHandler_class
    , qtscript_create_QAbstractUriResolver_class
    , qtscript_create_QXmlSerializer_class
    , qtscript_create_QSimpleXmlNodeModel_class
    , qtscript_create_QXmlFormatter_class
};

class com_trolltech_qt_xmlpatterns_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList com_trolltech_qt_xmlpatterns_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qt");
    list << QLatin1String("qt.xmlpatterns");
    return list;
}

void com_trolltech_qt_xmlpatterns_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qt")) {
    } else if (key == QLatin1String("qt.xmlpatterns")) {
        QScriptValue extensionObject = engine->globalObject();
        for (int i = 0; i < 14; ++i) {
            extensionObject.setProperty(qtscript_com_trolltech_qt_xmlpatterns_class_names[i],
                qtscript_com_trolltech_qt_xmlpatterns_class_functions[i](engine),
                QScriptValue::SkipInEnumeration);
        }
    } else {
        Q_ASSERT_X(false, "com_trolltech_qt_xmlpatterns::initialize", qPrintable(key));
    }
}
Q_EXPORT_STATIC_PLUGIN(com_trolltech_qt_xmlpatterns_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_com_trolltech_qt_xmlpatterns, com_trolltech_qt_xmlpatterns_ScriptPlugin)

