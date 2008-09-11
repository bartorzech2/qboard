#ifndef QBOARD_JSQBOARDVIEW_H_INCLUDED
#define QBOARD_JSQBOARDVIEW_H_INCLUDED 1

#include <qboard/ScriptQt.h> // convert_script_value<> and to/fromScriptValue()
#include <QObject>
#include <QScriptable>

#include <qboard/QBoardView.h>
namespace qboard {
    /**
        Used by toScriptValue() and fromScriptValue() to convert to/from
	native- and JS-side QBoardView objects.
    */
    template <>
    struct convert_script_value_f<QBoardView*>
    {
        QScriptValue operator()( QScriptEngine *, QBoardView * const & r) const;
        QBoardView * operator()( QScriptEngine *, const QScriptValue & args) const;
    };

    /**
        A JavaScript prototype class for wrapping QBoardView objects
	in QtScript code.
    */
    class JSQBoardView : public QObject, public QScriptable
    {
	Q_OBJECT
    public:
	explicit JSQBoardView( QObject * parent = 0 );
	virtual ~JSQBoardView();
	QBoardView * self();
    /**
        See QBoardView::board().
    */
    Q_INVOKABLE QBoard * board();
    /**
        See QBoardView::state().
    */
    Q_INVOKABLE GameState * state();
    /**
        See QBoardView::isGLMode().
    */
    Q_INVOKABLE bool isGLMode();
    /**
        See QBoardView::zoomOut().
    */
    Q_INVOKABLE void zoomOut();
    /**
        See QBoardView::zoomIn().
    */
    Q_INVOKABLE void zoomIn();
    /**
        See QBoardView::zoom().
    */
    Q_INVOKABLE void zoom( qreal );
    /**
        See QBoardView::zoomReset().
    */
    Q_INVOKABLE void zoomReset();
    /**
        See QBoardView::setHandDragMode().
    */
    Q_INVOKABLE void setHandDragMode(bool handMode);
    /**
        See QBoardView::clipCopySelected().
    */
    Q_INVOKABLE void clipCopySelected();
    /**
        See QBoardView::clipCutSelected().
    */
    Q_INVOKABLE void clipCutSelected();
    /**
        See QBoardView::clipPaste().
    */
    Q_INVOKABLE void clipPaste();
    /**
        See QBoardView::selectAll().
    */
    Q_INVOKABLE void selectAll();
    /**
        See QBoardView::setGLMode().
    */
    Q_INVOKABLE void setGLMode(bool);
    private:
	struct Impl;
	Impl * impl;
    };

} // namespace
#endif // QBOARD_JSQBOARDVIEW_H_INCLUDED
