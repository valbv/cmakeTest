#pragma once

#include <QSizePolicy>

#include "MovaviWidgetLib.h"

class QWidget;

namespace LayoutHelpers {

enum Alignment
{
	DEF = 0,
	L   = Qt::AlignLeft,
	R   = Qt::AlignRight,
	T   = Qt::AlignTop,
	B   = Qt::AlignBottom,
	HC  = Qt::AlignHCenter,
	VC  = Qt::AlignVCenter,
	LT  = L  | T,
	CT  = HC | T,
	RT  = R  | T,
	LC  = L  | VC,
	CC  = HC | VC,
	RC  = R  | VC,
	LB  = L  | B,
	CB  = HC | B,
	RB  = R  | B,
};

enum CellModifier : uint8_t
{
	CELL_INVALID,
	HSPAN, // растянуть левую соседнюю ячейку MGridLayout по горизонтали. Не имеет смысла, если слева нет ячейки с элементом
	VSPAN, // растянуть верхнюю соседнюю ячейку MGridLayout по вертикали. Не иммет смысла, если сверху нет ячейки с элементом
	SKIP   // пропустить ячейку. Не имеет смысла, если справа нет ячейки с элементом
};

enum SpacerPolicy : uint8_t
{
	SPACER_INVALID,
	// формат - horizontal QSizePolicy::Policy, vertical QSizePolicy::Policy
	STRETCH,
	SPACER_EE, // Expanding, Expanding
	SPACER_FE, // Fixed,     Expanding
	SPACER_EF, // Expanding, Fixed
	SPACER_FF, // Fixed,     Fixed
};

struct MOVAVIWIDGET_API BoxItem
{
	BoxItem(QWidget *    widget, Alignment align, int stretch = 0);
	BoxItem(QWidget *    widget, int stretch = 0);
	BoxItem(SpacerPolicy spacer, int stretch = 0);

	QWidget  *   widget = nullptr;
	Alignment    align{};
	SpacerPolicy spacer  = SPACER_INVALID;
	int          stretch = 0;
};

struct MOVAVIWIDGET_API GridItem
{
	GridItem(QWidget *    widget, Alignment align = {});
	GridItem(SpacerPolicy spacer, Alignment align = {});
	GridItem(CellModifier type);

	QWidget  *   widget = nullptr;
	Alignment    align{};
	CellModifier mod    = CELL_INVALID;
	SpacerPolicy spacer = SPACER_INVALID;
};

MOVAVIWIDGET_API std::pair<QSizePolicy::Policy, QSizePolicy::Policy> toSizePolicy(SpacerPolicy);

}
