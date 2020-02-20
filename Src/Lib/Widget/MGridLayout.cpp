#include "MGridLayout.h"

#include <cassert>
#include <QWidget>

#include "WidgetUtils.h"

void MGridLayout::addRow(std::initializer_list<QWidget *> const & widgets, int rowStretch)
{
	const int row = insertionRow();
	int col = 0;
	for (auto widget : widgets)
		addWidget(widget, row, col++, 1, 1);
	setRowStretch(row, rowStretch);
}

void MGridLayout::addRow(const std::initializer_list<LayoutHelpers::GridItem> & items, int rowStretch)
{
	using namespace LayoutHelpers;
	const int row = insertionRow();
	int col = 0;
	for (auto iter = items.begin(), end = items.end(); iter != end;)
	{
		auto calculateHSpan = [iter, end]() mutable {
			int span = 1;
			for (++iter; (iter != end) && (iter->mod == HSPAN); ++iter)
				++span;
			return span;
		};
		const GridItem  & item = *iter;
		const int colSpan = calculateHSpan();
		if (item.widget)
		{
			addWidget(item.widget, row, col, 1, colSpan, Qt::Alignment(item.align));
		}
		else if (item.spacer != SPACER_INVALID)
		{
			auto sizePolicy = toSizePolicy(item.spacer);
			addItem(new QSpacerItem(0, 0, sizePolicy.first, sizePolicy.second), row, col, 1, colSpan, Qt::Alignment(item.align));
		}
		else if (item.mod == VSPAN)
		{
			// ячейка уже могла быть растянута по горизонтали, нужно ее пропустить, чтобы не затереть следующим добавляемым элементом
			col += applyVSpan(row, col) - colSpan;// colSpan не меняем, чтобы не повлиять на iter
		}
		else if (item.mod == SKIP)
		{
			//ничего не делаем
		}
		col  += colSpan;
		iter += colSpan;
	}
	setRowStretch(row, rowStretch);
}

int MGridLayout::applyVSpan(int row, int col)
{
	auto upperItem = itemAtPosition(row - 1, col);
	auto pos = getItemPosition(upperItem);

	if (!pos.isValid())
		return 0;

	const auto itemAlign = upperItem->alignment();
	removeItem(upperItem);
	addItem(upperItem, pos.row, pos.column, pos.rowSpan + 1, pos.columnSpan, itemAlign);

	return pos.columnSpan - col + pos.column;
}

int MGridLayout::insertionRow() const
{
	// используем count(), т.к. isEmpty вовзращает true, если в лэйауте только spacerItem
	return count() ? rowCount() : 0; // пустой Grid выдает rowCount == 1
}

int MGridLayout::insertionCol() const
{
	// используем count(), т.к. isEmpty возвращает true, если в лэйауте только spacerItem
	return count() ? columnCount() : 0; // пустой Grid выдает columnCount == 1
}

void MGridLayout::addStretch(Qt::Orientations flags)
{
	using SP = QSizePolicy::Policy;
	const int row = insertionRow();
	const int col = insertionCol();

	if ((flags & Qt::Horizontal) && (flags & Qt::Vertical)) addSpacer(row, col, SP::Expanding, SP::Expanding);
	else if (flags & Qt::Horizontal)                        addSpacer(0,   col, SP::Expanding, SP::Fixed);
	else if (flags & Qt::Vertical)                          addSpacer(row, 0,   SP::Fixed,     SP::Expanding);
}

void MGridLayout::addSpacer(int row, int col, QSizePolicy::Policy hor, QSizePolicy::Policy vert)
{
	addItem(new QSpacerItem(0, 0, hor, vert), row, col);
}

void MGridLayout::autoFill(int colCount)
{
	QWidget * parent = parentWidget();
	if (!parent)
		return assert(!"layout haven't parentWidget");
	if (colCount < 1)
		return assert(!"colCount < 1, can't create grid");

	assert(!count());

	int col = 0;
	int row = 0;
	for (QObject * child : parent->children())
	{
		if (auto widget = qobject_cast<QWidget*>(child))
		{
			if (col == colCount)
			{
				col = 0;
				++row;
			}
			assert(indexOf(widget) == -1);
			addWidget(widget, row, col);
			++col;
		}
	}
}

void MGridLayout::clear()
{
	Util::clearLayout(this);
}

void MGridLayout::cleanup()
{
	Util::cleanupLayout(this);
}

GridPosition MGridLayout::getItemPosition(int index) const
{
	if (index < 0)
		return {};
	GridPosition pos;
	getItemPosition(index, &pos.row, &pos.column, &pos.rowSpan, &pos.columnSpan);
	return pos;
}

GridPosition MGridLayout::getItemPosition(QLayoutItem * item) const
{
	if (!item)
		return {};

	// todo: в Qt5.12 появился аналогичный метод
	// не самый оптимальный способ, но иначе нужен поиск во все 4 стороны, чтобы рассчитать начальную позицию и span-ы элемента
	auto indexOf = [this](QLayoutItem * item) {
		for (int i = 0; i < count(); ++i)
		{
			if (itemAt(i) == item)
				return i;
		}
		return -1;
	};

	const int index = indexOf(item);
	if (index == -1)
		return {};

	return getItemPosition(index);
}

GridPosition MGridLayout::getItemPosition(QWidget * widget) const
{
	if (!widget)
		return {};
	return getItemPosition(indexOf(widget));
}