#include "AbstractLayoutController.h"

#include <functional>

#include <QBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QMetaEnum>
#include <QMetaObject>

#include "MarginsToStr.h"

namespace {
constexpr int INVALID_INT_VALUE = -1;

bool isHorizontal(QBoxLayout * layout)
{
	return (layout->direction() == QBoxLayout::LeftToRight) || (layout->direction() == QBoxLayout::RightToLeft);
}
bool isVertical  (QBoxLayout * layout)
{
	return (layout->direction() == QBoxLayout::TopToBottom) || (layout->direction() == QBoxLayout::BottomToTop);
}

QString squashIntValues(int count, const std::function<int(int)>& indexToVal)
{
	QString result;
	result.reserve(count * 10); // с расчетом на двухразрядные числа
	for (int i = 0; i < count; ++i)
		result.append(QString().sprintf("[%d] %d, ", i, indexToVal(i)));
	if (count)
		result.chop(2);
	return result;
}

void applyIntValues(const QString& str, int layoutCount, const std::function<void(int, int)>& apply)
{
	const QVector<QStringRef> splitted = str.splitRef(',', QString::SkipEmptyParts);
	for (const QStringRef& curStr : splitted)
	{
		const int indexStart = curStr.indexOf('[');
		const int indexEnd   = curStr.indexOf(']', indexStart);

		if (indexStart == -1 || indexEnd == -1 || indexStart == indexEnd) //был некорректно передан список
			continue;
		const QStringRef          paramsStr = curStr.mid(indexStart + 1, indexEnd - indexStart - 1); // содержимое []
		const QVector<QStringRef> params    = paramsStr.split(' ', QString::SkipEmptyParts);

		const QStringRef    valueStr = curStr.right(curStr.size() - indexEnd - 1).trimmed();
		bool                success  = true;
		const int           value    = valueStr.isEmpty() ? 0 : valueStr.toInt(&success);

		if (!success)
			continue;

		for (const QStringRef& dirty : params)
		{
			const QStringRef& param = dirty.trimmed();
			if (param == "*") // для всех
			{
				for (int i = 0; i < layoutCount; ++i)
					apply(i, value);
			}
			else // по индексу
			{
				bool success = true;
				const int index = param.toInt(&success);
				if (success)
					apply(index, value);
			}
		}
	}
}

struct StaticMetaTypeRegistrator
{
	StaticMetaTypeRegistrator()
	{
		qRegisterMetaType<QLayout::SizeConstraint>();
	}
} s_metaTypeRegistrator;

} // namespace

QString AbstractLayoutController::layoutMargin() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return QString::number(INVALID_INT_VALUE);
	return Util::marginsToStr(layout->contentsMargins());
}

void AbstractLayoutController::setLayoutMargin(const QString & margins)
{
	if (QLayout* layout = getLayout())
		layout->setContentsMargins(Util::strToMargins(margins));
}

int AbstractLayoutController::layoutMarginLeft() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return INVALID_INT_VALUE;
	return layout->contentsMargins().left();
}

void AbstractLayoutController::setLayoutMarginLeft(int marginLeft)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	QMargins margins = layout->contentsMargins();
	margins.setLeft(marginLeft);
	layout->setContentsMargins(margins);
}

int AbstractLayoutController::layoutMarginRight() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return INVALID_INT_VALUE;
	return layout->contentsMargins().right();
}

void AbstractLayoutController::setLayoutMarginRight(int marginRight)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	QMargins margins = layout->contentsMargins();
	margins.setRight(marginRight);
	layout->setContentsMargins(margins);
}

int AbstractLayoutController::layoutMarginTop() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return INVALID_INT_VALUE;
	return layout->contentsMargins().top();
}

void AbstractLayoutController::setLayoutMarginTop(int marginTop)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	QMargins margins = layout->contentsMargins();
	margins.setTop(marginTop);
	layout->setContentsMargins(margins);
}

int AbstractLayoutController::layoutMarginBottom() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return INVALID_INT_VALUE;
	return layout->contentsMargins().bottom();
}

void AbstractLayoutController::setLayoutMarginBottom(int marginBottom)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	QMargins margins = layout->contentsMargins();
	margins.setBottom(marginBottom);
	layout->setContentsMargins(margins);
}

QString AbstractLayoutController::layoutSpacing() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return QString::number(INVALID_INT_VALUE);
	if (QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(layout))
		return QString::number(boxLayout->spacing());
	if (QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout))
		return QString("%1 %2").arg(gridLayout->horizontalSpacing()).arg(gridLayout->verticalSpacing());
	if (QFormLayout* formLayout = qobject_cast<QFormLayout*>(layout))
		return QString("%1 %2").arg(formLayout->horizontalSpacing()).arg(formLayout->verticalSpacing());
	return QString::number(INVALID_INT_VALUE);
}

void AbstractLayoutController::setLayoutSpacing(const QString & str)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	const QString fixedString = str.simplified();
	const QVector<QStringRef> splitted = fixedString.splitRef(' ', QString::SkipEmptyParts);
	if (splitted.size() == 1)
		layout->setSpacing(splitted[0].toInt());
	else if (splitted.size() == 2)
	{
		setLayoutHSpacing(splitted[0].toInt());
		setLayoutVSpacing(splitted[1].toInt());
	}
}

int AbstractLayoutController::layoutHSpacing() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return INVALID_INT_VALUE;
	if (QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout))
		return gridLayout->horizontalSpacing();
	if (QFormLayout* formLayout = qobject_cast<QFormLayout*>(layout))
		return formLayout->horizontalSpacing();
	if (QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(layout))
	{
		if (isHorizontal(boxLayout))
			return boxLayout->spacing();
	}
	return INVALID_INT_VALUE;
}

void AbstractLayoutController::setLayoutHSpacing(int hSpacing)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	if (QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout))
		return gridLayout->setHorizontalSpacing(hSpacing);
	if (QFormLayout* formLayout = qobject_cast<QFormLayout*>(layout))
		return formLayout->setHorizontalSpacing(hSpacing);
	if (QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(layout))
	{
		if (isHorizontal(boxLayout))
			return boxLayout->setSpacing(hSpacing);
	}
}

int AbstractLayoutController::layoutVSpacing() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return INVALID_INT_VALUE;
	if (QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout))
		return gridLayout->verticalSpacing();
	if (QFormLayout* formLayout = qobject_cast<QFormLayout*>(layout))
		return formLayout->verticalSpacing();
	if (QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(layout))
	{
		if (isVertical(boxLayout))
			return boxLayout->spacing();
	}
	return INVALID_INT_VALUE;
}

void AbstractLayoutController::setLayoutVSpacing(int vSpacing)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	if (QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout))
		return gridLayout->setVerticalSpacing(vSpacing);
	if (QFormLayout* formLayout = qobject_cast<QFormLayout*>(layout))
		return formLayout->setVerticalSpacing(vSpacing);
	if (QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(layout))
	{
		if (isVertical(boxLayout))
			return boxLayout->setSpacing(vSpacing);
	}
}

Qt::Alignment AbstractLayoutController::layoutAlign() const
{
	if (QLayout* layout = getLayout())
		return layout->alignment();
	return{};
}

void AbstractLayoutController::setLayoutAlign(Qt::Alignment align)
{
	if (QLayout* layout = getLayout())
		layout->setAlignment(align);
}

QString AbstractLayoutController::layoutWidgetAlign() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return{};
	const int size = layout->count();
	if (size < 1)
		return{};
	QString widgetNameSelectors;
	QString classNameSelectors;
	const QMetaEnum meta = QMetaEnum::fromType<Qt::Alignment>();
	for (int i = 0; i < size; ++i)
	{
		QLayoutItem*   item   = layout->itemAt(i);
		const QWidget* widget = item->widget();
		if (!widget)
			continue;
		const Qt::Alignment align    = item->alignment();
		const QByteArray    alignStr = meta.valueToKeys(align);

		if (widget->objectName().isEmpty())
		{
			const QString selector = QString("[.%1] %2, ").arg(widget->metaObject()->className()).replace(':', '-').arg(alignStr.data());
			if (!classNameSelectors.contains(selector))
				classNameSelectors += selector;
		}
		else // при наличии виджетов с одинаковыми именами, но разным align, результат будет такой, что сеттер будет его интерпретировать иначе (возможно при установке align в обход AbstractLayoutController)
			widgetNameSelectors += QString("[#%1] %2, ").arg(widget->objectName()).arg(alignStr.data());
	}
	QString result = classNameSelectors + widgetNameSelectors;
	if (result.endsWith(", "))
		result.chop(2);
	return result;
}

void AbstractLayoutController::setLayoutWidgetAlign(const QString& str)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	const QMetaEnum meta = QMetaEnum::fromType<Qt::Alignment>();
	const QVector<QStringRef> splitted = str.splitRef(',', QString::SkipEmptyParts);

	for (const QStringRef& curStr : splitted)
	{
		const int indexStart = curStr.indexOf('[');
		const int indexEnd   = curStr.indexOf(']', indexStart);

		if (indexStart == -1 || indexEnd == -1 || indexStart == indexEnd) //был некорректно передан список
			continue;
		const QStringRef          paramsStr = curStr.mid(indexStart + 1, indexEnd - indexStart - 1); // содержимое []
		const QVector<QStringRef> params    = paramsStr.split(' ', QString::SkipEmptyParts);

		const QStringRef    alignStr = curStr.right(curStr.size() - indexEnd - 1);
		const int           alignVal = meta.keysToValue(alignStr.toUtf8());
		const Qt::Alignment align    = alignVal == -1  ? Qt::Alignment() : static_cast<Qt::Alignment>(alignVal);

		auto applyAlign = [align, layout](const auto& func) {
			for (int i = 0; i < layout->count(); ++i)
			{
				QLayoutItem* item = layout->itemAt(i);
				if (item->widget() && func(item->widget()))
					item->setAlignment(align);
			}
		};
		for (const QStringRef& dirty : params)
		{
			const QStringRef& param = dirty.trimmed();
			if (param.startsWith('#')) //по имени
			{
				const QStringRef widgetName = param.mid(1);
				applyAlign([&widgetName](QWidget* widget) { return widget->objectName() == widgetName; });
			}
			else if (param == "*") // для всех виджетов
			{
				applyAlign([](QWidget*) { return true; });
			}
			else if (param.startsWith('.')) //для конкретного типа
			{
				const QByteArray className = param.mid(1).toUtf8().replace('-', ':');
				applyAlign([&className](QWidget* widget) { return widget->metaObject()->className() == className; });
			}
			else // для типа и наследников
			{
				const QByteArray className = param.toUtf8().replace('-', ':');
				applyAlign([&className](QWidget* widget) { return widget->inherits(className); });
			}
		}
	}
}

QString AbstractLayoutController::layoutColMinWidth() const
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout || layout->isEmpty())
		return {};
	return squashIntValues(layout->columnCount(), [layout](int i) {
		return layout->columnMinimumWidth(i);
	});
}

void AbstractLayoutController::setLayoutColMinWidth(const QString& str)
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout)
		return;
	applyIntValues(str, layout->columnCount(), [layout](int index, int width) {
		layout->setColumnMinimumWidth(index, width);
	});
}

QString AbstractLayoutController::layoutRowMinHeight() const
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout || layout->isEmpty())
		return {};
	return squashIntValues(layout->rowCount(), [layout](int i) {
		return layout->rowMinimumHeight(i);
	});
}

void AbstractLayoutController::setLayoutRowMinHeight(const QString& str)
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout)
		return;
	applyIntValues(str, layout->rowCount(), [layout](int index, int height) {
		layout->setRowMinimumHeight(index, height);
	});
}

QString AbstractLayoutController::layoutStretch() const
{
	auto layout = qobject_cast<QBoxLayout*>(getLayout());
	if (!layout || layout->isEmpty())
		return {};
	return squashIntValues(layout->count(), [layout](int i) {
		return layout->stretch(i);
	});
}

void AbstractLayoutController::setLayoutStretch(const QString& str)
{
	auto layout = qobject_cast<QBoxLayout*>(getLayout());
	if (!layout)
		return;
	applyIntValues(str, layout->count(), [layout](int index, int stretch) {
		layout->setStretch(index, stretch);
	});
}

QString AbstractLayoutController::layoutColStretch() const
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout || layout->isEmpty())
		return {};
	return squashIntValues(layout->columnCount(), [layout](int i) {
		return layout->columnStretch(i);
	});
}

void AbstractLayoutController::setLayoutColStretch(const QString& str)
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout)
		return;
	applyIntValues(str, layout->columnCount(), [layout](int index, int stretch) {
		layout->setColumnStretch(index, stretch);
	});
}

QString AbstractLayoutController::layoutRowStretch() const
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout || layout->isEmpty())
		return {};
	return squashIntValues(layout->rowCount(), [layout](int i) {
		return layout->rowStretch(i);
	});
}

void AbstractLayoutController::setLayoutRowStretch(const QString& str)
{
	auto layout = qobject_cast<QGridLayout*>(getLayout());
	if (!layout)
		return;
	applyIntValues(str, layout->rowCount(), [layout](int index, int stretch) {
		layout->setRowStretch(index, stretch);
	});
}

QLayout::SizeConstraint AbstractLayoutController::layoutSizeConstraint() const
{
	QLayout* layout = getLayout();
	if (!layout)
		return QLayout::SetDefaultConstraint;
	return layout->sizeConstraint();
}

void AbstractLayoutController::setLayoutSizeConstraint(QLayout::SizeConstraint value)
{
	QLayout* layout = getLayout();
	if (!layout)
		return;
	layout->setSizeConstraint(value);
}

