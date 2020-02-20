#include "MSliderMagneticPoint.h"

#include <QAbstractSlider>
#include <QMouseEvent>
#include <QStyle>

MSliderMagneticPoint::MSliderMagneticPoint(QAbstractSlider * slider, int magneticValue, int magneticDistPx)
	: QObject(slider)
	, m_slider(slider)
	, m_magneticValue(magneticValue)
	, m_magneticDist(magneticDistPx)
	, m_previousPos(valueToPos(magneticValue))
{
	m_slider->installEventFilter(this);
}

MSliderMagneticPoint::~MSliderMagneticPoint()
{
	m_slider->removeEventFilter(this);
}

int MSliderMagneticPoint::posToValue(int pos) const
{
	const int span = m_slider->orientation() == Qt::Horizontal ? m_slider->width() : m_slider->height();
	const bool invert = m_slider->orientation() == Qt::Vertical;
	return QStyle::sliderValueFromPosition(m_slider->minimum(), m_slider->maximum(), pos, span, invert);
}

int MSliderMagneticPoint::valueToPos(int value) const
{
	const int span = m_slider->orientation() == Qt::Horizontal ? m_slider->width() : m_slider->height();
	const bool invert = m_slider->orientation() == Qt::Vertical;
	return QStyle::sliderPositionFromValue(m_slider->minimum(), m_slider->maximum(), value, span, invert);
}

bool MSliderMagneticPoint::eventFilter(QObject * watched, QEvent * event)
{
	if (watched != m_slider || event->type() != QEvent::MouseMove)
		return QObject::eventFilter(watched, event);

	if (!m_slider->isSliderDown() || !m_slider->hasFocus())
		return QObject::eventFilter(watched, event);

	QMouseEvent * moveEvent = static_cast<QMouseEvent*>(event);
	auto pickPoint = [this](QPoint const & pt) {
		return m_slider->orientation() == Qt::Horizontal ? pt.x() : pt.y();
	};

	const int magneticPos = valueToPos(m_magneticValue);
	const int curPos      = pickPoint(moveEvent->pos());
	const int direction   = curPos - m_previousPos;
	if (direction == 0) // на маке приходят лишние события мыши с повторяющимися координатами, что приводит к отскоку слайдера к позиции курсора
		return true;

	const bool inside = (curPos >= magneticPos - m_magneticDist) && (curPos <= magneticPos + m_magneticDist);
	const bool moveToMagneticPoint = (direction * (magneticPos - curPos)) > 0;
	bool shouldMagnetize = false;

	if (moveToMagneticPoint)
		shouldMagnetize = inside && m_needMagnetize;
	else
		m_needMagnetize = !inside;

	m_previousPos = curPos;

	if (!inside)
		return QObject::eventFilter(watched, event);
	if (shouldMagnetize)
		m_slider->setSliderPosition(m_magneticValue);
	else
		m_slider->setSliderPosition(posToValue(curPos)); // если вызывать базовый eventFilter, то слайдер не встает на нужную позицию (например, слайдер -255, 255 и точка 0), приходится ставить вручную
	return true;
}
