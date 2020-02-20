#pragma once

#include <initializer_list>
#include <QGridLayout>

#include "LayoutHelpers.h"

#include "MovaviWidgetLib.h"

// Обертка над QGridLayout
// Позволяет добавлять виджеты построчно (addRow), не заботясь индексах для каждого элемента
// При небольших размерах позволяет гораздо более наглядно задавать элементы, чем в QGridLayout
// Пример:
// layout->addRow({modesLabel,     modesFrame,      HSPAN         });
// layout->addRow({opacityLabel,   opacitySlider,   opacitySpinBox});
// layout->addRow({outlineLabel,   outlineCheckBox, SKIP          });
// layout->addStretch();

struct GridPosition
{
	int row = -1;
	int column = -1;
	int rowSpan = 0;
	int columnSpan = 0;

	bool isValid() const { return row != -1 && column != -1; }
};

class MOVAVIWIDGET_API MGridLayout : public QGridLayout
{
	Q_OBJECT
public:
	using QGridLayout::QGridLayout;

	void addRow(const std::initializer_list<QWidget *> & widgets, int rowStretch = 0);
	void addRow(const std::initializer_list<LayoutHelpers::GridItem> & items, int rowStretch = 0);

	void addStretch(Qt::Orientations = Qt::Horizontal | Qt::Vertical);

	void addSpacer(int row, int col, QSizePolicy::Policy hor, QSizePolicy::Policy vert);

	// автоматически добавляет себе все дочерние видежеты своего parent-а
	// удобен, когда нужна равномерная матрица из элементов
	void autoFill(int colCount);

	void clear();
	void cleanup();

	using QGridLayout::getItemPosition;
	GridPosition getItemPosition(int index) const;
	GridPosition getItemPosition(QLayoutItem *) const;
	GridPosition getItemPosition(QWidget *) const;

private:
	int applyVSpan(int row, int col);

	int insertionRow() const;
	int insertionCol() const;
};