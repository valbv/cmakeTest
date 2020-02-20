#include "WidgetUtils.h"

#include <cassert>
#include <memory>

#include <QApplication>
#include <QString>
#include <QVariant>
#include <QStyle>
#include <QWidget>
#include <QLayout>
#include <QAbstractSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QAbstractButton>

#include "MLabel.h"
#include "MFrame.h"
#include "MDoubleSlider.h"
#include "MBoxLayout.h"
#include "LayoutController.h"

namespace Util {

LayoutController * installLayoutController(QWidget * target)
{
	assert(target);
	if (auto layoutController = LayoutController::getLayoutController(target))
		return layoutController;
	return new LayoutController(target);
}

LayoutController * installLayoutController(QLayout * target, QWidget * parent)
{
	assert(target);
	assert(parent);
	if (auto layoutController = LayoutController::getLayoutController(target))
		return layoutController;
	return new LayoutController(target, parent);
}

QFrame * createSpacer(QWidget * parent, const QString & name)
{
	return createWidget<QFrame>(name, parent);
}

void clearLayout(QLayout * layout)
{
	while (auto item = layout->takeAt(0))
	{
		if (auto inner = item->layout())
			clearLayout(inner);
		delete item;
	}
}

void cleanupLayout(QLayout * layout)
{
	while (auto item = layout->takeAt(0))
	{
		if (auto inner = item->layout())
			cleanupLayout(inner);
		if (auto widget = item->widget())
			delete widget;
		delete item;
	}
}

void repolishWidget(QWidget * widget)
{
	assert(widget);
	qApp->style()->unpolish(widget);
	qApp->style()->polish(widget);
}

void repolishWidgetRecursive(QWidget * widget)
{
	assert(widget);
	if (!widget)
		return;

	for (QWidget * child : widget->findChildren<QWidget *>())
		repolishWidget(child);

	repolishWidget(widget);
}

void applyProperty(QWidget * widget, const char * name, const QVariant & value)
{
	assert(widget);
	const QVariant curValue = widget->property(name);
	if (curValue != value)
	{
		widget->setProperty(name, value);
		repolishWidget(widget);
	}
}

void applyStyleHint(QWidget * widget, const QString & value)
{
	applyProperty(widget, "styleHint", value);
}

void bindWidgets(QAbstractSlider * slider, QSpinBox * spinBox)
{
	QObject::connect(slider,              &QAbstractSlider::rangeChanged,  spinBox,        &QSpinBox::setRange);
	QObject::connect(slider,              &QAbstractSlider::valueChanged,  spinBox,        &QSpinBox::setValue);
	QObject::connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), slider,  &QAbstractSlider::setValue);

	QObject::connect(slider, &QAbstractSlider::valueChanged, spinBox, [slider, spinBox](int value) {
		const QString tooltip = spinBox->prefix() + QString::number(value) + spinBox->suffix();
		slider->setToolTip(tooltip);
	});
}

void bindWidgets(MDoubleSlider * slider, QDoubleSpinBox * spinBox)
{
	auto guard = std::make_shared<bool>(false);

	QObject::connect(slider, &MDoubleSlider::doubleRangeChanged, spinBox, &QDoubleSpinBox::setRange);
	QObject::connect(slider, &MDoubleSlider::doubleValueChanged, spinBox, [spinBox, guard](double value) {
		if (*guard)
			return;
		*guard = true;
		spinBox->setValue(value);
		*guard = false;
	});
	QObject::connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), slider, [slider, guard](double value) {
		if (*guard)
			return;
		*guard = true;
		slider->setDoubleValue(value);
		*guard = false;
	});

	QObject::connect(slider, &MDoubleSlider::doubleValueChanged, [slider, spinBox](double value){
		const QString tooltip = spinBox->prefix() + QString::number(value, 'g', spinBox->decimals()) + spinBox->suffix();
		slider->setToolTip(tooltip);
	});
}

void bindWidgets(MLabel * source, QAbstractButton * destination)
{
	QObject::connect(source, &MLabel::clicked, destination, &QAbstractButton::click);
}

QScrollArea * wrapWithScrollArea(QWidget * widget, Qt::ScrollBarPolicy hPolicy, Qt::ScrollBarPolicy vPolicy)
{
	auto scrollArea = new QScrollArea(widget->parentWidget());
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(hPolicy);
	scrollArea->setVerticalScrollBarPolicy(vPolicy);
	scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
	scrollArea->setWidget(widget);
	return scrollArea;
}

QScrollArea * wrapWithHScrollArea(QWidget * widget, Qt::ScrollBarPolicy hPolicy)
{
	return wrapWithScrollArea(widget, hPolicy, Qt::ScrollBarAlwaysOff);
}

QScrollArea * wrapWithVScrollArea(QWidget * widget, Qt::ScrollBarPolicy vPolicy)
{
	return wrapWithScrollArea(widget, Qt::ScrollBarAlwaysOff, vPolicy);
}

MFrame * wrapWithMFrame(QWidget * widget, const QString & name, Qt::Edge edge)
{
	assert(widget);
	assert(widget->parentWidget()); // если нет parent-а, то этот виджет может быть создан на стеке или хранится в умном указателе

	if (!widget || !widget->parentWidget())
		return nullptr;

	MFrame * frame = createWidget<MFrame>(name, widget->parentWidget());

	using namespace LayoutHelpers;
	if (edge == Qt::LeftEdge)
		createZeroLayout<MHBoxLayout>(frame)->addItems({ STRETCH, widget  });
	else if (edge == Qt::RightEdge)
		createZeroLayout<MHBoxLayout>(frame)->addItems({ widget,  STRETCH });
	else if (edge == Qt::TopEdge)
		createZeroLayout<MVBoxLayout>(frame)->addItems({ STRETCH, widget  });
	else if (edge == Qt::BottomEdge)
		createZeroLayout<MVBoxLayout>(frame)->addItems({ widget,  STRETCH });
	else
		createZeroLayout<MHBoxLayout>(frame)->addWidget(widget);
	return frame;
}

MFrame * wrapWithMFrame(QWidget * widget, Qt::Edge edge)
{
	assert(widget);
	assert(!widget->objectName().isEmpty());
	return wrapWithMFrame(widget, widget->objectName() + "Frame", edge);
}

QWidget * createDelimiter(Qt::Orientation orientation, QWidget * parent)
{
	static const QMap<Qt::Orientation, QString> orientation2styleHint = {
		{ Qt::Horizontal, "horizontal" },
		{ Qt::Vertical  , "vertical"   }
	};

	auto * delimiter = new QFrame(parent);
	applyProperty(delimiter, "delimiter", orientation2styleHint.value(orientation));
	return delimiter;
}

QWidget * createDelimiter(Qt::Orientation orientation, const QString & name, QWidget * parent)
{
	auto widget = createDelimiter(orientation, parent);
	widget->setObjectName(name);
	return widget;
}

void setTabOrder(const QList<QWidget*>& widgets)
{
	if (widgets.size() < 2)
		return assert(!"required at least 2 widget for tab order");
	for (int i = 0; i < widgets.size() - 1; ++i)
		QWidget::setTabOrder(widgets[i], widgets[i + 1]);
}

void resetHoverState(QWidget * widget)
{
	assert(widget);
	widget->setAttribute(Qt::WA_UnderMouse, widget->rect().contains(widget->mapFromGlobal(QCursor::pos())));
}

namespace Details {

QString removeClassMemberPrefix(const char * widgetName)
{
	// префикс удаляем, а не добавляем с помощью макросов, чтобы не было проблем с IDE (иначае она не распознает поле класса в параметре макроса создания виджета)
	assert((widgetName[0] == 'm') && (widgetName[1] == '_'));
	return QString(widgetName + 2);
}

}

}
