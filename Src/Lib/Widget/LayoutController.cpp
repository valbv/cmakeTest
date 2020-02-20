#include "LayoutController.h"

#include <cassert>

#include <QLayout>
#include <QVariant>

namespace {
constexpr const char* LAYOUT_CONTROLLER_PROPERTY_NAME = "_layoutController";
}

LayoutController::LayoutController(QWidget * parent)
	: QWidget(parent)
	, m_widget(parent)
{
	assert(parent);
	hide();
}

LayoutController::LayoutController(QLayout * layout, QWidget * parent)
	: QWidget(parent)
	, m_layout(layout)
{
	assert(layout);
	hide();

	assert(layout->property(LAYOUT_CONTROLLER_PROPERTY_NAME).isNull());
	layout->setProperty(LAYOUT_CONTROLLER_PROPERTY_NAME, QVariant::fromValue(this));
}

LayoutController::~LayoutController()
{
	if (m_layout)
		m_layout->setProperty(LAYOUT_CONTROLLER_PROPERTY_NAME, QVariant());
}

LayoutController * LayoutController::getLayoutController(QLayout * layout)
{
	return layout->property(LAYOUT_CONTROLLER_PROPERTY_NAME).value<LayoutController*>();
}

LayoutController * LayoutController::getLayoutController(QWidget * widget)
{
	return widget->findChild<LayoutController*>(QString(), Qt::FindDirectChildrenOnly);
}

QLayout* LayoutController::getLayout() const
{
	if (m_widget)
		return m_widget->layout();
	if (m_layout)
		return m_layout;
	return nullptr;
}