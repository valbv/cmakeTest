#include "MBoxLayout.h"

#include <cassert>

#include <QWidget>

#include "WidgetUtils.h"

MBoxLayout * MBoxLayout::autoFill()
{
	QWidget * parent = parentWidget();
	if (!parent)
	{
		assert(!"layout hasn't parentWidget");
		return this;
	}
	for (QObject * child : parent->children())
	{
		if (auto widget = qobject_cast<QWidget*>(child))
		{
			assert(indexOf(widget) == -1);
			addWidget(widget);
		}
	}
	return this;
}

MBoxLayout * MBoxLayout::addStretch(int stretch)
{
	QBoxLayout::addStretch(stretch);
	return this;
}

void MBoxLayout::addWidget(QWidget * widget, Qt::Alignment align)
{
	addWidget(widget, 0, align);
}

void MBoxLayout::addWidgets(const std::initializer_list<QWidget*> & widgets)
{
	for (auto widget : widgets)
		addWidget(widget);
}

void MBoxLayout::addItems(const std::initializer_list<LayoutHelpers::BoxItem> & items)
{
	for (const auto & item : items)
	{
		if (item.widget)
			addWidget(item.widget, item.stretch, Qt::Alignment(item.align));
		else if (item.spacer == LayoutHelpers::STRETCH)
			addStretch(item.stretch);
	}
}

void MBoxLayout::clear()
{
	Util::clearLayout(this);
}

void MBoxLayout::cleanup()
{
	Util::cleanupLayout(this);
}

MHBoxLayout::MHBoxLayout(QWidget * parent)
	: MBoxLayout(Direction::LeftToRight, parent)
{}

MVBoxLayout::MVBoxLayout(QWidget * parent)
	: MBoxLayout(Direction::TopToBottom, parent)
{}