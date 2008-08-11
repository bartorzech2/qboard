#ifndef QBOARD_PIECEAPPEARANCECHOOSER_H_INCLUDED
#define QBOARD_PIECEAPPEARANCECHOOSER_H_INCLUDED 1

#include <QWidget>
#include "Serializable.h"
#include "GameState.h"
class QGraphicsView;
class PieceAppearanceWidget : public QWidget, public Serializable
{
Q_OBJECT
public:
    PieceAppearanceWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
    virtual ~PieceAppearanceWidget();
    /** Serializes this object to. */
    virtual bool serialize( S11nNode & dest ) const;
    /** Deserializes src to this object. */
    virtual bool deserialize( S11nNode const & src );
    virtual bool event( QEvent * e );
    GameState & state();
    GameState const & state() const;
    QGraphicsView * view();
    void applyCurrentTemplate( GamePiece * target );
public Q_SLOTS:
    /**
       Clears the state of the widget.
    */
    void clear();
private:
    void setupUI();
    class Impl;
    Impl * impl;
};
// Register PieceAppearanceWidget with s11n:
#define S11N_TYPE PieceAppearanceWidget
#define S11N_BASE_TYPE Serializable
#define S11N_TYPE_NAME "PieceAppearanceWidget"
#include <s11n.net/s11n/reg_s11n_traits.hpp>
#define S11N_TYPE PieceAppearanceWidget
#define S11N_TYPE_NAME "PieceAppearanceWidget"
#define S11N_SERIALIZE_FUNCTOR Serializable_s11n
#include <s11n.net/s11n/reg_s11n_traits.hpp>


#endif // QBOARD_PIECEAPPEARANCECHOOSER_H_INCLUDED
