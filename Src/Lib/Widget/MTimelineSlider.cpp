#include "MTimelineSlider.h"

#include <algorithm>

#include <QMouseEvent>
#include <QToolTip>
#include <QTime>

#include <cassert>

MTimelineSlider::MTimelineSlider(QWidget *parent/* = 0*/)
	: MSlider(parent)
	, m_startTime(0)
	, m_endTime(0)
	, m_leftBound(-1)
	, m_rightBound(-1)
	, m_time(0)
	, m_boundingMode(FullSliderWidth)
{
	//если менять m_time только в valueChanged, то в обработчике сигнала sliderMoved значения value() и time() будут несогласованы, т.к. value меняется перед вызовом sliderMoved в QAbstractSlider::setValue
	//порядок сигналов в пользовательском коде: timeChanged -> sliderMoved(только при пользовательском вводе) -> valueChanged
	connect(this, &QAbstractSlider::sliderMoved,  this, &MTimelineSlider::updateTime);
	connect(this, &QAbstractSlider::valueChanged, this, &MTimelineSlider::updateTime);

	setMinimum(0);
	setMaximum(1000);

	setMouseTracking(true);
}

void MTimelineSlider::setStyleTag( const QString &str )
{
	m_styleTag = str;
}

void MTimelineSlider::setDisplayFormatList( const QStringList &list )
{
	m_displayFormatList = list;
}

void MTimelineSlider::setTimeRange(qint64 min, qint64 max)
{
	setStartTime(min);
	setEndTime(max);
}

void MTimelineSlider::setStartTime(qint64 startTime)
{
	if (startTime == m_startTime)
		return;
	m_startTime = startTime;

	// Reapplying bounds to thumb because it might has been miscalculated in case setBoundRange() was called before setStartTime()/setEndTime().
	if (m_leftBound >= 0)
		defaultThumb()->setMinimum(time2position(m_leftBound));
	if (m_rightBound >= 0)
		defaultThumb()->setMaximum(time2position(m_rightBound));

	setTime(m_time);
}

void MTimelineSlider::setEndTime(qint64 endTime)
{
	if (endTime == m_endTime)
		return;
	m_endTime = endTime;

	// Reapplying bounds to thumb because it might has been miscalculated in case setBoundRange() was called before setStartTime()/setEndTime().
	if (m_leftBound >= 0)
		defaultThumb()->setMinimum(time2position(m_leftBound));
	if (m_rightBound >= 0)
		defaultThumb()->setMaximum(time2position(m_rightBound));

	setTime(m_time);
}

void MTimelineSlider::setBoundRange(qint64 left, qint64 right)
{
	setLeftBound(left);
	setRightBound(right);

	assert( ( m_leftBound == -1 || m_rightBound == -1 ) || m_rightBound > m_leftBound ); // If both bounds are set then right bound must be greater than the left one.
}

void MTimelineSlider::removeBoundRange()
{
	m_leftBound = m_rightBound = -1;
	defaultThumb()->setMinimum(minimum());
	defaultThumb()->setMaximum(maximum());
}

void MTimelineSlider::setLeftBound(qint64 left)
{
	if (m_leftBound == left)
		return;
	m_leftBound = left;
	defaultThumb()->setMinimum(time2position(left)); // bound thumb
	setTime(m_time);
}

void MTimelineSlider::setRightBound(qint64 right)
{
	if (m_rightBound == right)
		return;
	m_rightBound = right;
	defaultThumb()->setMaximum(time2position(right)); // bound thumb
	setTime(m_time);
}

void MTimelineSlider::setBoundingMode(BoundingMode mode)
{
	m_boundingMode = mode;
}

void MTimelineSlider::setTime(qint64 time)
{
	setValue(time2position(time));
}

void MTimelineSlider::mouseMoveEvent(QMouseEvent *event)
{
	QString timeFormat = "hh:mm:ss";
	int width = this->width() - 2*grooveOffset();
	int posX = event->pos().x() - grooveOffset();
	// считаем прогресс для данной позиции
	double progressPercent = posX * 1.0 / width;
	// Проверяем выход за границы.
	if (posX > width)
		progressPercent = 1.0;
	else if (posX < 0)
		progressPercent = 0.0;

	QTime time(0,0,0,0);
	qint64 startOffset = ( m_boundingMode == FullSliderWidth ) ? 0 : boundsStartOffset();
	time = time.addSecs( (startOffset + progressPercent * (endTimeBounded() - startTimeBounded())) / 1000LL );

	// Определяем какой формат представления времени подходит лучше всего.
	if ( m_displayFormatList.size() > 0 )
	{
		double  selectedError  = 1e300;

		QStringList::iterator I = m_displayFormatList.begin();
		QStringList::iterator E = m_displayFormatList.end();
		for ( ; I != E ; ++I )
		{
			QString visibleString = time.toString(*I);
			QTime  visibleTime    = QTime::fromString( visibleString , *I );
			double visibleError   = time.msecsTo( visibleTime );

			if (visibleError < 0 )
				visibleError = -visibleError;

			if ( visibleError < selectedError )
			{
				timeFormat = *I;
				selectedError  = visibleError;
			}
		}
	}

	QToolTip::showText(mapToGlobal( event->pos() ), time.toString( timeFormat ), this, rect());

	MSlider::mouseMoveEvent( event );
}

int MTimelineSlider::time2position(qint64 time) const
{
	// учёт ограничений на значение времени
	time = qBound(m_startTime, time, m_endTime);

	assert( ( m_leftBound == -1 || m_rightBound == -1 ) || m_rightBound > m_leftBound ); // If both bounds are set then right bound must be greater than the left one.

	// учёт bound-ограничений
	if (m_leftBound >= 0)
		time = qMax(m_leftBound, time);
	if (m_rightBound >= 0)
		time = qMin(time, m_rightBound);

	time -= boundsStartOffset();
	const qint64 deltaTimeBounded = endTimeBounded() - startTimeBounded();
	if (deltaTimeBounded == 0)
		return 0;

	const double progress = static_cast<double>(time) / deltaTimeBounded;
	return static_cast<int>(progress * (maximum() - minimum()) + 0.5);
}

qint64 MTimelineSlider::position2time(int pos) const
{
	// учёт ограничений на значение
	pos = qBound(minimum(), pos, maximum());

	// учёт bound-ограничений
	if (m_leftBound >= 0 && pos == defaultThumb()->minimum()) // handle on its minimum value
		return m_leftBound;
	if (m_rightBound >= 0 && pos == defaultThumb()->maximum()) // handle on its maximum value
		return m_rightBound;

	double progress = static_cast<double>(pos)/(maximum() - minimum());
	return boundsStartOffset() + static_cast<qint64>(progress * (endTimeBounded() - startTimeBounded()) + 0.5);
}

qint64 MTimelineSlider::startTimeBounded() const
{
	return (m_leftBound >= 0 && m_boundingMode == FullSliderWidth) ? m_leftBound : m_startTime;
}

qint64 MTimelineSlider::endTimeBounded() const
{
	return (m_rightBound >= 0 && m_boundingMode == FullSliderWidth) ? m_rightBound : m_endTime;
}

qint64 MTimelineSlider::boundsStartOffset() const
{
	return (m_leftBound >= 0 && m_boundingMode == FullSliderWidth) ? m_leftBound : 0LL;
}

void MTimelineSlider::reset()
{
	this->blockSignals(true);
	setValue(0);
	this->blockSignals(false);
	m_time = 0;
}

void MTimelineSlider::updateTime(int value)
{
	qint64 time = position2time(value);
	if (time == m_time)
		return;
	m_time = time;
	emit timeChanged(m_time);
}
