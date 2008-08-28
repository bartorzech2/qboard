#ifndef QBOARDPLUGIN_H_INCLUDED
#define QBOARDPLUGIN_H_INCLUDED


#include <QObject>
class QString;
class GameState;
class QWidget;

struct QBoardPluginInfo
{
    QString name;
    QString license;
    QString url;
    QString author;
    QString version;
    QString comments;
};

/**
   Basic plugin interface for QBoard plugins.

   Unfortunately, clients MUST implement all virtual functions.
   Qt's plugin system requires/assumes that virtuals in this class
   are abstract.
*/
class QBoardPlugin
{
public:
    virtual ~QBoardPlugin() {}
    /**
       Implementations may offer a widget for use with the main UI.
       The widget is then owned by QBoard, which is likely to destroy
       it at some point (using delete or QObject::deleteLater()).
    */
    virtual QWidget * widget() = 0;
    /**
       Sets the current game state. Does not change ownership
       of the state.
    */
    virtual void setGameState( GameState & ) = 0;

    /**
       Should return a populated QBoardPluginInfo object containing
       plugin-specific data.
    */
    virtual QBoardPluginInfo pluginInfo() const = 0;
};
Q_DECLARE_INTERFACE(QBoardPlugin,
		    "com.google.code/p/qboard/QBoardPlugin/1.0"
		    //"com.google.code.p.qboard.QBoardPlugin/1.0"
		    );


class QBoardBasePlugin : public QObject,
			 public QBoardPlugin
{
Q_OBJECT
Q_INTERFACES(QBoardPlugin)
public:
    QBoardBasePlugin();
    virtual ~QBoardBasePlugin();
    virtual QWidget * widget();
    virtual void setGameState(GameState &);
    virtual QBoardPluginInfo pluginInfo() const;
protected:
    QBoardPluginInfo & pluginInfo();
    GameState * gameState();
private:
    struct Impl;
    Impl * impl;
};

#endif // QBOARDPLUGIN_H_INCLUDED
