#ifndef CGMEJOE_H_INCLUDED
#define CGMEJOE_H_INCLUDED 1

#include <QObject>
#include "QBoardPlugin.h"
class CGMEJoe : public QObject,
		public QBoardPlugin
    
{
Q_OBJECT
Q_INTERFACES(QBoardPlugin)
public:
    CGMEJoe();
    virtual ~CGMEJoe();
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
