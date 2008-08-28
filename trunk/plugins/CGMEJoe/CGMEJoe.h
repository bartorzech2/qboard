#ifndef CGMEJOE_H_INCLUDED
#define CGMEJOE_H_INCLUDED 1

#if 0
#  include "QBoardPlugin.h"
#else
// i can't get my -Includes to be passed on to moc, so we do an ugly kludge:
#  include "../../src/QBoardPlugin.h"
#endif

class CGMEJoe : public QBoardBasePlugin
{
Q_OBJECT
Q_INTERFACES(QBoardPlugin)
public:
    CGMEJoe();
    virtual ~CGMEJoe();
    virtual QWidget * widget();
private Q_SLOTS:
    void widgetDestroyed();
private:
    struct Impl;
    Impl * impl;
};

#endif // CGMEJOE_H_INCLUDED
