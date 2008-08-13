#ifndef CGMEJOE_H_INCLUDED
#define CGMEJOE_H_INCLUDED 1

#if 0
#  include "QBoardPlugin.h"
#else
// i can't get my -Includes to be passed on to moc, so we do an ugly kludge:
#  include "../../src/QBoardPlugin.h"
#endif

class CGMEJoe : public QObject, QBoardPlugin
{
Q_OBJECT
Q_INTERFACES(QBoardPlugin)
public:
    CGMEJoe();
    virtual ~CGMEJoe();
    virtual QWidget * widget();
    virtual void setGameState(GameState *);

};

#endif // CGMEJOE_H_INCLUDED
