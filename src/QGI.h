#ifndef __QGI_H__
#define __QGI_H__

struct QGITypes
{
	/**
		All QBoard QGraphicsItems should have a type entry here.
	*/
	enum Types {
	_TypesStart = QGraphicsItem::UserType + 100,
	GamePiece,
	Html,
	LineNode,
	LineNodeBinder
	};
};
#endif // __QGI_H__
