#include "LayoutHelpers.h"

#include <cassert>

namespace  LayoutHelpers {

BoxItem::BoxItem(QWidget * widget, Alignment align, int stretch)
	: widget(widget)
	, align(align)
	, stretch(stretch)
{}
BoxItem::BoxItem(QWidget * widget, int stretch)
	: BoxItem(widget, Alignment(), stretch)
{}
BoxItem::BoxItem(SpacerPolicy spacer, int stretch)
	: spacer(spacer)
	, stretch(stretch)
{
	assert(spacer == STRETCH);
}

GridItem::GridItem(QWidget * widget, Alignment align)
	: widget(widget)
	, align(align)
{}
GridItem::GridItem(CellModifier mod)
	: mod(mod)
{}
GridItem::GridItem(SpacerPolicy spacer, Alignment align)
	: align(align)
	, spacer(spacer)
{
	assert(spacer != STRETCH);
}

std::pair<QSizePolicy::Policy, QSizePolicy::Policy> toSizePolicy(SpacerPolicy spacer)
{
	switch (spacer)
	{
	case(STRETCH):        // [[fallthrough]]
	case(SPACER_EE):      return {QSizePolicy::Expanding, QSizePolicy::Expanding};
	case(SPACER_EF):      return {QSizePolicy::Expanding, QSizePolicy::Fixed};
	case(SPACER_FE):      return {QSizePolicy::Fixed,     QSizePolicy::Expanding};
	case(SPACER_FF):      return {QSizePolicy::Fixed,     QSizePolicy::Fixed};
	case(SPACER_INVALID): return {};
	}
	return{};
}

}
