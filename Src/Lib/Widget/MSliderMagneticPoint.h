#pragma once

#include <QObject>

#include "MovaviWidgetLib.h"

/**
 * @class MSliderMagneticPoint
 * @brief MSliderMagneticPoint позволяет магнититься к точке на слайдере.
 *
 * Магнитится только при движении к точке, если слайдер был вне зоны примагничивания.
 * Если слайдер находится в зоне примагничивания, то он не будет магнитится.
 */

class QAbstractSlider;

class MOVAVIWIDGET_API MSliderMagneticPoint : public QObject
{
public:
	/**
	 * @brief принимает указетель на слайдер, вычисляет расстояние для примагничивания, устанавливает фильтр текущему слайдеру.
	 * @a slider слайдер, который надо магнитить
	 * @a magneticValue - значение слайдера, к которому надо магнититься
	 * @a magneticDistPx - интервал примагничивания в пикселях
	*/
	MSliderMagneticPoint(QAbstractSlider * slider, int magneticValue, int magneticDistPx);
	~MSliderMagneticPoint();

protected:
	bool eventFilter(QObject * watched, QEvent * event) override;

private:
	int valueToPos(int value) const;
	int posToValue(int pos) const;

private:
	QAbstractSlider * m_slider;
	int m_magneticValue;
	int m_magneticDist;
	int m_previousPos = 0;
	bool m_needMagnetize = false;
};
