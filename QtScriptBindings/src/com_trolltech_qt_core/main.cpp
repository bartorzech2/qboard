#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtCore/QDebug>

#include <qwaitcondition.h>
#include <qxmlstream.h>
#include <qtscriptconcurrent.h>
#include <qdatetime.h>
#include <qbytearraymatcher.h>
#include <quuid.h>
#include <qbytearray.h>
#include <qtscriptconcurrent.h>
#include <qlocale.h>
#include <qabstractitemmodel.h>
#include <qxmlstream.h>
#include <qmutex.h>
#include <qtextcodec.h>
#include <qrect.h>
#include <qstringmatcher.h>
#include <qsemaphore.h>
#include <qdir.h>
#include <qsystemsemaphore.h>
#include <qnamespace.h>
#include <qfileinfo.h>
#include <qxmlstream.h>
#include <qxmlstream.h>
#include <qtscriptconcurrent.h>
#include <qbitarray.h>
#include <qtscriptconcurrent.h>
#include <qtextcodec.h>
#include <qtscriptconcurrent.h>
#include <qdiriterator.h>
#include <qlibraryinfo.h>
#include <qtconcurrentexception.h>
#include <qobject.h>
#include <qxmlstream.h>
#include <qtextstream.h>
#include <qbasictimer.h>
#include <qrect.h>
#include <qtextboundaryfinder.h>
#include <qtscriptconcurrent.h>
#include <qxmlstream.h>
#include <qsize.h>
#include <qpoint.h>
#include <qrunnable.h>
#include <qpoint.h>
#include <qdatastream.h>
#include <qcryptographichash.h>
#include <qsize.h>
#include <qcoreevent.h>
#include <qurl.h>
#include <qxmlstream.h>
#include <qtextcodec.h>
#include <qtscriptconcurrent.h>
#include <qxmlstream.h>
#include <qcoreevent.h>
#include <qtimeline.h>
#include <qsignalmapper.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <qiodevice.h>
#include <qtimer.h>
#include <qeventloop.h>
#include <qfilesystemwatcher.h>
#include <qthreadpool.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qsocketnotifier.h>
#include <qcoreevent.h>
#include <qprocess.h>
#include <qbuffer.h>
#include <qfile.h>
#include <qtemporaryfile.h>

QScriptValue qtscript_create_QWaitCondition_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamReader_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFutureVoid_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTime_class(QScriptEngine *engine);
QScriptValue qtscript_create_QByteArrayMatcher_class(QScriptEngine *engine);
QScriptValue qtscript_create_QUuid_class(QScriptEngine *engine);
QScriptValue qtscript_create_QByteArray_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFutureIterator_class(QScriptEngine *engine);
QScriptValue qtscript_create_QLocale_class(QScriptEngine *engine);
QScriptValue qtscript_create_QModelIndex_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamWriter_class(QScriptEngine *engine);
QScriptValue qtscript_create_QMutex_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTextDecoder_class(QScriptEngine *engine);
QScriptValue qtscript_create_QRect_class(QScriptEngine *engine);
QScriptValue qtscript_create_QStringMatcher_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSemaphore_class(QScriptEngine *engine);
QScriptValue qtscript_create_QDir_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSystemSemaphore_class(QScriptEngine *engine);
QScriptValue qtscript_create_Qt_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFileInfo_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamEntityResolver_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamAttributes_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFutureSynchronizerVoid_class(QScriptEngine *engine);
QScriptValue qtscript_create_QBitArray_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFutureWatcher_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTextEncoder_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFutureSynchronizer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QDirIterator_class(QScriptEngine *engine);
QScriptValue qtscript_create_QLibraryInfo_class(QScriptEngine *engine);
QScriptValue qtscript_create_QtConcurrent_class(QScriptEngine *engine);
QScriptValue qtscript_create_QObject_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamEntityDeclaration_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTextStream_class(QScriptEngine *engine);
QScriptValue qtscript_create_QBasicTimer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QRectF_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTextBoundaryFinder_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFutureWatcherVoid_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamAttribute_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSizeF_class(QScriptEngine *engine);
QScriptValue qtscript_create_QPointF_class(QScriptEngine *engine);
QScriptValue qtscript_create_QRunnable_class(QScriptEngine *engine);
QScriptValue qtscript_create_QPoint_class(QScriptEngine *engine);
QScriptValue qtscript_create_QDataStream_class(QScriptEngine *engine);
QScriptValue qtscript_create_QCryptographicHash_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSize_class(QScriptEngine *engine);
QScriptValue qtscript_create_QEvent_class(QScriptEngine *engine);
QScriptValue qtscript_create_QUrl_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamNamespaceDeclaration_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTextCodec_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFuture_class(QScriptEngine *engine);
QScriptValue qtscript_create_QXmlStreamNotationDeclaration_class(QScriptEngine *engine);
QScriptValue qtscript_create_QChildEvent_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTimeLine_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSignalMapper_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTranslator_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSettings_class(QScriptEngine *engine);
QScriptValue qtscript_create_QIODevice_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTimer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QEventLoop_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFileSystemWatcher_class(QScriptEngine *engine);
QScriptValue qtscript_create_QThreadPool_class(QScriptEngine *engine);
QScriptValue qtscript_create_QCoreApplication_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTimerEvent_class(QScriptEngine *engine);
QScriptValue qtscript_create_QSocketNotifier_class(QScriptEngine *engine);
QScriptValue qtscript_create_QDynamicPropertyChangeEvent_class(QScriptEngine *engine);
QScriptValue qtscript_create_QProcess_class(QScriptEngine *engine);
QScriptValue qtscript_create_QBuffer_class(QScriptEngine *engine);
QScriptValue qtscript_create_QFile_class(QScriptEngine *engine);
QScriptValue qtscript_create_QTemporaryFile_class(QScriptEngine *engine);

static const char * const qtscript_com_trolltech_qt_core_class_names[] = {
    "QWaitCondition"
    , "QXmlStreamReader"
    , "QFutureVoid"
    , "QTime"
    , "QByteArrayMatcher"
    , "QUuid"
    , "QByteArray"
    , "QFutureIterator"
    , "QLocale"
    , "QModelIndex"
    , "QXmlStreamWriter"
    , "QMutex"
    , "QTextDecoder"
    , "QRect"
    , "QStringMatcher"
    , "QSemaphore"
    , "QDir"
    , "QSystemSemaphore"
    , "Qt"
    , "QFileInfo"
    , "QXmlStreamEntityResolver"
    , "QXmlStreamAttributes"
    , "QFutureSynchronizerVoid"
    , "QBitArray"
    , "QFutureWatcher"
    , "QTextEncoder"
    , "QFutureSynchronizer"
    , "QDirIterator"
    , "QLibraryInfo"
    , "QtConcurrent"
    , "QObject"
    , "QXmlStreamEntityDeclaration"
    , "QTextStream"
    , "QBasicTimer"
    , "QRectF"
    , "QTextBoundaryFinder"
    , "QFutureWatcherVoid"
    , "QXmlStreamAttribute"
    , "QSizeF"
    , "QPointF"
    , "QRunnable"
    , "QPoint"
    , "QDataStream"
    , "QCryptographicHash"
    , "QSize"
    , "QEvent"
    , "QUrl"
    , "QXmlStreamNamespaceDeclaration"
    , "QTextCodec"
    , "QFuture"
    , "QXmlStreamNotationDeclaration"
    , "QChildEvent"
    , "QTimeLine"
    , "QSignalMapper"
    , "QTranslator"
    , "QSettings"
    , "QIODevice"
    , "QTimer"
    , "QEventLoop"
    , "QFileSystemWatcher"
    , "QThreadPool"
    , "QCoreApplication"
    , "QTimerEvent"
    , "QSocketNotifier"
    , "QDynamicPropertyChangeEvent"
    , "QProcess"
    , "QBuffer"
    , "QFile"
    , "QTemporaryFile"
};

typedef QScriptValue (*QtBindingCreator)(QScriptEngine *engine);
static const QtBindingCreator qtscript_com_trolltech_qt_core_class_functions[] = {
    qtscript_create_QWaitCondition_class
    , qtscript_create_QXmlStreamReader_class
    , qtscript_create_QFutureVoid_class
    , qtscript_create_QTime_class
    , qtscript_create_QByteArrayMatcher_class
    , qtscript_create_QUuid_class
    , qtscript_create_QByteArray_class
    , qtscript_create_QFutureIterator_class
    , qtscript_create_QLocale_class
    , qtscript_create_QModelIndex_class
    , qtscript_create_QXmlStreamWriter_class
    , qtscript_create_QMutex_class
    , qtscript_create_QTextDecoder_class
    , qtscript_create_QRect_class
    , qtscript_create_QStringMatcher_class
    , qtscript_create_QSemaphore_class
    , qtscript_create_QDir_class
    , qtscript_create_QSystemSemaphore_class
    , qtscript_create_Qt_class
    , qtscript_create_QFileInfo_class
    , qtscript_create_QXmlStreamEntityResolver_class
    , qtscript_create_QXmlStreamAttributes_class
    , qtscript_create_QFutureSynchronizerVoid_class
    , qtscript_create_QBitArray_class
    , qtscript_create_QFutureWatcher_class
    , qtscript_create_QTextEncoder_class
    , qtscript_create_QFutureSynchronizer_class
    , qtscript_create_QDirIterator_class
    , qtscript_create_QLibraryInfo_class
    , qtscript_create_QtConcurrent_class
    , qtscript_create_QObject_class
    , qtscript_create_QXmlStreamEntityDeclaration_class
    , qtscript_create_QTextStream_class
    , qtscript_create_QBasicTimer_class
    , qtscript_create_QRectF_class
    , qtscript_create_QTextBoundaryFinder_class
    , qtscript_create_QFutureWatcherVoid_class
    , qtscript_create_QXmlStreamAttribute_class
    , qtscript_create_QSizeF_class
    , qtscript_create_QPointF_class
    , qtscript_create_QRunnable_class
    , qtscript_create_QPoint_class
    , qtscript_create_QDataStream_class
    , qtscript_create_QCryptographicHash_class
    , qtscript_create_QSize_class
    , qtscript_create_QEvent_class
    , qtscript_create_QUrl_class
    , qtscript_create_QXmlStreamNamespaceDeclaration_class
    , qtscript_create_QTextCodec_class
    , qtscript_create_QFuture_class
    , qtscript_create_QXmlStreamNotationDeclaration_class
    , qtscript_create_QChildEvent_class
    , qtscript_create_QTimeLine_class
    , qtscript_create_QSignalMapper_class
    , qtscript_create_QTranslator_class
    , qtscript_create_QSettings_class
    , qtscript_create_QIODevice_class
    , qtscript_create_QTimer_class
    , qtscript_create_QEventLoop_class
    , qtscript_create_QFileSystemWatcher_class
    , qtscript_create_QThreadPool_class
    , qtscript_create_QCoreApplication_class
    , qtscript_create_QTimerEvent_class
    , qtscript_create_QSocketNotifier_class
    , qtscript_create_QDynamicPropertyChangeEvent_class
    , qtscript_create_QProcess_class
    , qtscript_create_QBuffer_class
    , qtscript_create_QFile_class
    , qtscript_create_QTemporaryFile_class
};

class com_trolltech_qt_core_ScriptPlugin : public QScriptExtensionPlugin
{
public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

QStringList com_trolltech_qt_core_ScriptPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("qt");
    list << QLatin1String("qt.core");
    return list;
}

void com_trolltech_qt_core_ScriptPlugin::initialize(const QString &key, QScriptEngine *engine)
{
    if (key == QLatin1String("qt")) {
    } else if (key == QLatin1String("qt.core")) {
        QScriptValue extensionObject = engine->globalObject();
        for (int i = 0; i < 69; ++i) {
            extensionObject.setProperty(qtscript_com_trolltech_qt_core_class_names[i],
                qtscript_com_trolltech_qt_core_class_functions[i](engine),
                QScriptValue::SkipInEnumeration);
        }
    } else {
        Q_ASSERT_X(false, "com_trolltech_qt_core::initialize", qPrintable(key));
    }
}
Q_EXPORT_STATIC_PLUGIN(com_trolltech_qt_core_ScriptPlugin)
Q_EXPORT_PLUGIN2(qtscript_com_trolltech_qt_core, com_trolltech_qt_core_ScriptPlugin)

