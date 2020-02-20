#pragma once

#include <initializer_list>
#include <QBoxLayout>

#include "LayoutHelpers.h"

#include "MovaviWidgetLib.h"

// Обертка над QBoxLayout
// Напрямую использовать MBoxLayout нужно только в случае нестандартного направления лэйаута.
// В большинстве случаев нужно использовать MHBoxLayout (аналог QHBoxLayout), MVBoxLayout (аналог QVBoxLayout)
// Пример: layout->addItems({titleLabel, STRETCH, controlFrame});

class MOVAVIWIDGET_API MBoxLayout : public QBoxLayout
{
	Q_OBJECT
public:
	using QBoxLayout::QBoxLayout;

	// автоматически добавляет себе все дочерние видежеты своего parent-а
	// полезен, когда у нас созданы все виджеты в нужном порядке и между ними не нужны stretch-и
	MBoxLayout * autoFill();

	// аналогичен QBoxLyaout::addStretch, но его можно использовать в связке с autoFill в однострочных выражениях
	MBoxLayout * addStretch(int stretch = 0);

	using QBoxLayout::addWidget;
	void addWidget(QWidget *, Qt::Alignment);

	// addWidgets, addItems могут быть полезны при заполнении небольшим колчеством виджетов, если есть необходимость написать более компактно, чем при использовании addWidget
	void addWidgets(const std::initializer_list<QWidget*> &);
	void addItems  (const std::initializer_list<LayoutHelpers::BoxItem> &);

	// выкидывает все элементы (QLayoutItem-ы удаляются, виджеты - нет)
	void clear();

	// удаляет все элементы
	void cleanup();
};

class MOVAVIWIDGET_API MHBoxLayout : public MBoxLayout
{
	Q_OBJECT
public:
	MHBoxLayout(QWidget * parent = nullptr);
};

class MOVAVIWIDGET_API MVBoxLayout : public MBoxLayout
{
	Q_OBJECT
public:
	MVBoxLayout(QWidget * parent = nullptr);
};