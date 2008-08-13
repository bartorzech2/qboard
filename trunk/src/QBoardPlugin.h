#ifndef QBOARDPLUGIN_H_INCLUDED
#define QBOARDPLUGIN_H_INCLUDED


#include <QObject>
class QString;
class GameState;
class QWidget;

// class QBoardPluginInfo
// {
//     QBoardPluginInfo();
//     ~QBoardPluginInfo();
// };
/**
   Basic plugin interface for QBoard plugins.

   Unfortunately, clients MUST implement all virtual functions.
   Qt's plugin system requires/assumes that virtuals in this class
   are abstract.
*/
class QBoardPlugin
{
public:
    QBoardPlugin() {}
    virtual ~QBoardPlugin() {}
    /**
       Implementations may offer a widget for use with the main UI.
       The widget is owned by this object, and this object should be
       prepared to:

       - Clean up with widget.

       - Accomodate that a parent widget destroys this widget before
       this object gets to clean it up.
    */
    virtual QWidget * widget() = 0;
    /**
       Sets the current game state. Does not change ownership
       of the state.
    */
    virtual void setGameState( GameState * ) = 0;
};
Q_DECLARE_INTERFACE(QBoardPlugin,
		    "com.google.code/p/qboard/QBoardPlugin/1.0"
		    //"com.google.code.p.qboard.QBoardPlugin/1.0"
		    );
#endif // QBOARDPLUGIN_H_INCLUDED
