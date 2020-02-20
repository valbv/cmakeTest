#include "PanelItem.h"

#include "PanelWidget.h"

PanelItem::PanelItem(QGraphicsItem* parent)
    : QGraphicsProxyWidget(parent)
    , m_panelWidget(new PanelWidget(nullptr))
{
    setWidget(m_panelWidget);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
}

PanelItem::~PanelItem() = default;
