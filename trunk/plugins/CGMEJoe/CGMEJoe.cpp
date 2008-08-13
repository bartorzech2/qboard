#include <QtGui>
#include "CGMEJoe.h"

#include <QDebug>



CGMEJoe::CGMEJoe() : QBoardPlugin()
{
    qDebug() << "CGMEJoe()";
#if 0
    this->setPluginName( "CGME Joe Gameset" );
    this->setPluginVersion( __DATE__ " " __TIME__ );
    this->setPluginLicense( "GPLv3" );
    this->setPluginUrl( "http://code.google.com/p/qboard" );
    this->setPluginAuthor( "Stephan Beal" );
#endif
}

CGMEJoe::~CGMEJoe()
{
    qDebug() << "~CGMEJoe()";
}

QWidget * CGMEJoe::widget()
{
    return 0;
}
void CGMEJoe::setGameState(GameState *)
{
}


Q_EXPORT_PLUGIN2(CGMEJoe, CGMEJoe)
