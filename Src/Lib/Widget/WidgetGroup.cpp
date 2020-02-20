#include "WidgetGroup.h"

#include <QWidget>

WidgetGroup::WidgetGroup(QList<QWidget *> const & widgets)
	:m_widgets(widgets)
{
}

void WidgetGroup::addWidget(QWidget * widget)
{
	m_widgets.append(widget);
}
void WidgetGroup::addWidgets(QList<QWidget *> const & widgets)
{
	m_widgets += widgets;
}
void WidgetGroup::removeWidget(QWidget * widget)
{
	m_widgets.removeOne(widget);
}

QList<QWidget *> WidgetGroup::widgets() const
{
	return m_widgets;
}

void WidgetGroup::setVisible(bool visible)
{
	for (auto widget : m_widgets)
		widget->setVisible(visible);
}

void WidgetGroup::setEnabled(bool enabled)
{
	for (auto widget : m_widgets)
		widget->setEnabled(enabled);
}

void WidgetGroup::hide()    { setVisible(false); }
void WidgetGroup::show()    { setEnabled(true);  }
void WidgetGroup::enable()  { setEnabled(true);  }
void WidgetGroup::disable() { setEnabled(false); }