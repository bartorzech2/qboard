#include <qboard/JSQBoardView.h>
#include <qboard/QBoardView.h>
#include <qboard/GameState.h>
#include <qboard/QBoard.h>

Q_DECLARE_METATYPE(QBoardView*);

namespace qboard {
    QScriptValue convert_script_value_f<QBoardView*>::operator()(QScriptEngine *eng,
                                                               QBoardView * const & in) const
    {
        return in ? eng->newQObject(in) : eng->nullValue();
    }

    QBoardView * convert_script_value_f<QBoardView*>::operator()( QScriptEngine *,
                                                              const QScriptValue & args ) const
    {
        return qobject_cast<QBoardView*>(args.toQObject());
    }

    struct JSQBoardView::Impl
    {
	// Add JSQBoardView-internal data here.
	Impl()
	{}
	~Impl()
	{}
    };

#define SELF(RV) QBoardView *self = this->self();     QScriptEngine * js = this->engine();     if(!self || !js) return RV;
#define SELFQOBJ(RV) SELF(RV);     QObject *selfobj = qobject_cast<QObject*>(self);     if(!selfobj) return RV;

    JSQBoardView::JSQBoardView( QObject * parent )
    : QObject(parent),
      QScriptable(),
      impl(new Impl)
    {
    }

    JSQBoardView::~JSQBoardView()
    {
	delete impl;
    }

    QBoardView * JSQBoardView::self()
    {
	QBoardView * s = qscriptvalue_cast<QBoardView*>(this->thisObject());
	if( ! s )
	{
	    this->context()->throwError("JSQBoardView::self() got a null pointer.");
	}
	return s;
    }

    QBoard * JSQBoardView::board()
    {
	SELF(0);
	return &self->board();
    }
    GameState * JSQBoardView::state()
    {
	SELF(0);
	return &self->state();
    }
    bool JSQBoardView::isGLMode()
    {
	SELF(false);
	return self->isGLMode();
    }
    void JSQBoardView::zoomOut()
    {
	SELF();
	self->zoomOut();
    }
    void JSQBoardView::zoomIn()
    {
	SELF();
	self->zoomIn();
    }
    void JSQBoardView::zoom( qreal z )
    {
	SELF();
	self->zoom( z );
    }
    void JSQBoardView::zoomReset()
    {
	SELF();
	self->zoomReset();
    }
    void JSQBoardView::setHandDragMode(bool handMode)
    {
	SELF();
	self->setHandDragMode(handMode);
    }
    void JSQBoardView::clipCopySelected()
    {
	SELF();
	self->clipCopySelected();
    }
    void JSQBoardView::clipCutSelected()
    {
	SELF();
	return self->clipCutSelected();
    }
    void JSQBoardView::clipPaste()
    {
	SELF();
	return self->clipPaste();
    }
    void JSQBoardView::selectAll()
    {
	SELF();
	return self->selectAll();
    }
    void JSQBoardView::setGLMode(bool b)
    {
	SELF();
	return self->setGLMode(b);
    }
#undef SELF
#undef SELFQOBJ
} // namespace
