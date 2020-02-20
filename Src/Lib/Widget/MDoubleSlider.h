#pragma once
#include <functional>

#include "MCustomSlider.h"

#include "MovaviWidgetLib.h"

/**
 * @brief MDoubleSlider позволяет использовать функционал QSlider для работы с вещественными значениями линейно
 * распределёнными по слайдеру, используя заданный вещественный множитель. По умолчанию множитель = 1
 * так же можно задать произвольные функции преобразования
 * Удобен в связке с QDoubleSpinBox (MDoubleSpinBox)
 */

class MOVAVIWIDGET_API MDoubleSlider : public MCustomSlider
{
	Q_OBJECT

public:
	using ToIntFunc    = std::function<int(double, const MDoubleSlider*)>;
	using ToDoubleFunc = std::function<double(int, const MDoubleSlider*)>;

	MDoubleSlider(QWidget * = nullptr);

	void setConvertFunc(const ToIntFunc&, const ToDoubleFunc&);
	void setMultiplier(double);

	double doubleValue() const;

	int    toInt(double) const;
	double toDouble(int) const;

public slots:
	void setDoubleValue(double);

signals:
	/// Вещественное значение слайдера с учётом множителя
	void doubleValueChanged(double);
	/// Вещественные границы слайдера с учётом множителя
	void doubleRangeChanged(double, double);

private:
	ToIntFunc m_toInt;
	ToDoubleFunc m_toDouble;
};
