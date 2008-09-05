#ifndef CGMEJOE_H_INCLUDED
#define CGMEJOE_H_INCLUDED 1

#include <QObject>
//#include "../../include/qboard/QBoardPlugin.h"
#include <qboard/QBoardPlugin.h>
class JSEditor : public QObject,
		public QBoardPlugin
    
{
Q_OBJECT
Q_INTERFACES(QBoardPlugin)
public:
    JSEditor();
    virtual ~JSEditor();
    virtual QWidget * widget();
    virtual void setGameState(GameState &);

private Q_SLOTS:
    void widgetDestroyed();
    void evalJS();
private:
    struct Impl;
    Impl * impl;
};

#endif // CGMEJOE_H_INCLUDED
