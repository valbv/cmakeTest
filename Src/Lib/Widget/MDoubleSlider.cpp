#include "MDoubleSlider.h"

#include <cassert>
#include <cmath>

MDoubleSlider::MDoubleSlider(QWidget * parent) : MCustomSlider(parent)
{
	connect(this, &QSlider::valueChanged, [this](int value) {
		emit doubleValueChanged(doubleValue());
	});
	connect(this, &QSlider::rangeChanged, [this](int min, int max) {
		emit doubleRangeChanged(toDouble(min), toDouble(max));
	});
}

void MDoubleSlider::setConvertFunc(const ToIntFunc & toInt, const ToDoubleFunc & toDouble)
{
	assert(toInt && toDouble);
	m_toInt    = toInt;
	m_toDouble = toDouble;

	emit doubleRangeChanged(this->toDouble(minimum()), this->toDouble(maximum()));
	emit doubleValueChanged(doubleValue());
}

void MDoubleSlider::setMultiplier(double multiplier)
{
	if (qFuzzyIsNull(multiplier))
	{
		assert(!"Zero multiplier is not valid for MDoubleSlider");
		return;
	}
	setConvertFunc(
		[multiplier](double value, const MDoubleSlider *) { return std::lround(value / multiplier); },
		[multiplier](int    value, const MDoubleSlider *) { return double(value) * multiplier; }
	);
}

void MDoubleSlider::setDoubleValue(double value)
{
	setValue(toInt(value));
}

double MDoubleSlider::doubleValue() const
{
	return toDouble(value());
}

int MDoubleSlider::toInt(double value) const
{
	if (m_toInt)
		return m_toInt(value, this);
	return int(value);
}

double MDoubleSlider::toDouble(int value) const
{
	if (m_toDouble)
		return m_toDouble(value, this);
	return double(value);
}
