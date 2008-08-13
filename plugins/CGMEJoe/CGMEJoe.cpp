#include <QtGui>
#include <QLabel>
#include <QDebug>

#include "CGMEJoe.h"
#include "GameState.h"

struct CGMEJoe::Impl
{
    QWidget * wid;
    GameState * gs;
    Impl() :
	wid(0),
	gs(0)
    {
    }
    ~Impl()
    {
	delete wid;
	wid = 0;
    }
};

CGMEJoe::CGMEJoe() : QBoardPlugin(),
		     impl(new Impl)
{
    qDebug() << "CGMEJoe()";
}

CGMEJoe::~CGMEJoe()
{
    qDebug() << "~CGMEJoe()";
    if( impl->wid )
    {
	QObject::disconnect( impl->wid, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()) );
    }
    delete impl;
}

void CGMEJoe::widgetDestroyed()
{
    impl->wid = 0;
}
QWidget * CGMEJoe::widget()
{
    if( ! impl->wid )
    {
	QLabel * lbl = new QLabel("Hi, world");
	impl->wid = lbl;
	connect( impl->wid, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()) );
    }
    return impl->wid;
}
void CGMEJoe::setGameState(GameState *s)
{
    impl->gs = s;
}

QBoardPluginInfo CGMEJoe::pluginInfo()
{
    QBoardPluginInfo in;
    in.name = "CGME Joe Gameset";
    in.version = __DATE__ " " __TIME__;
    in.license = "GPLv3";
    in.url = "http://code.google.com/p/qboard";
    in.author = "Stephan Beal";
    return in;
}

Q_EXPORT_PLUGIN2(CGMEJoe, CGMEJoe)
