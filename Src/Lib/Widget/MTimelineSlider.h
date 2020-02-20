#pragma once

#include "MSlider.h"

#include "MovaviWidgetLib.h"

/**
 * @class MTimelineSlider
 * @brief Элемент управления: слайдер с отображением временной позиции под курсором и с возможностями MSlider
 *
 * @note Слайдер устроен таким образом, что позволяет обрабатывать значения, превосходящие int.
 * Для этого слайдер использует собственную градацию (табуляцию), а значения в каждой позиции определяется
 * значением линейной функции в точке слайдера:
 * startTime() + (endTime() - startTime()) / (maximum() - minumum()) * value()
 * Поскольку интерфейс QSlider (и MSlider соответственно) не позволяет виртуально переопределить minimum, maximum и value,
 * то работа с данным слайдером в общем случае ведётся через другой интерфейс (startTime, endTime, time)
 *
 * 0       40%             1
 * |--------|--------------|
 * S        T              E
 */
class MOVAVIWIDGET_API MTimelineSlider : public MSlider
{
public:
	enum BoundingMode
	{
		FullSliderWidth, // Bounds that are set in setBoundRange() will take effect (time values will be affected), but visually everything will be the same (thumb will be moving without limits).
		BoundedSpan      // Thumb movements will be limited by bounds set in setBoundRange() as well as time values.
	};

private:
	Q_OBJECT
	Q_PROPERTY (qint64 startTime	READ startTime	WRITE setStartTime)
	Q_PROPERTY (qint64 endTime		READ endTime	WRITE setEndTime)
	Q_PROPERTY (qint64 leftBound	READ leftBound	WRITE setLeftBound)
	Q_PROPERTY (qint64 rightBound	READ rightBound	WRITE setRightBound)
	Q_PROPERTY (qint64 time			READ time		WRITE setTime		NOTIFY timeChanged)

	/*Свойство, задающее стиль отображения.*/
	Q_PROPERTY(QString styleTag					READ styleTag			WRITE setStyleTag )
	/*Свойство, задающее форматы для отображения данных.*/
	Q_PROPERTY(QStringList displayFormatList	READ displayFormatList	WRITE setDisplayFormatList )

public:
	MTimelineSlider(QWidget *parent = 0);
	virtual ~MTimelineSlider() {}

	QString     styleTag()          { return m_styleTag; }
	QStringList displayFormatList() { return m_displayFormatList; }

	void setStyleTag          ( const QString & );
	void setDisplayFormatList ( const QStringList & );

	/// @name Установка регулируемых значений @{
	void setTimeRange(qint64, qint64);
	void setStartTime(qint64);
	void setEndTime(qint64);
	void setTime(qint64);
	void setBoundRange(qint64, qint64); // Перемещение ползунка
	void removeBoundRange();            // возможно только в заданных
	void setLeftBound(qint64);          // границах
	void setRightBound(qint64);         //
	void setBoundingMode(BoundingMode);
	/// @}

	/// @name Получение регулируемых значений @{
	qint64 startTime() const { return m_startTime; }
	qint64 endTime() const { return m_endTime; }
	qint64 time() const { return m_time; }
	qint64 leftBound() const { return m_leftBound; }
	qint64 rightBound() const { return m_rightBound; }
	BoundingMode boundingMode() const { return m_boundingMode; }
	/// @}

public:
	/// Преобразовывает положение слайдера во временную метку (в миллисекундах).
	qint64 position2time(int pos) const;
	/// Преобразовывает временную метку в положение слайдера
	int time2position(qint64 time) const;
	/// Сброс позиции на 0 без испускания сигнала.
	void reset();

private:
	inline qint64 startTimeBounded() const;
	inline qint64 endTimeBounded() const;
	inline qint64 boundsStartOffset() const;

protected:
	void mouseMoveEvent(QMouseEvent *event);

signals:
	void timeChanged(qint64);

private slots:
	/**
	  * @brief Реакция на изменение значения слайдера.
	  * В обработчике необходимо расчитать реальное значение и уведомить об изменении подписчиков.
	  * @param[in] value - новое (измененное) значение.
	  */
	void updateTime(int value);

private:
	qint64 m_startTime;
	qint64 m_endTime;
	qint64 m_leftBound;
	qint64 m_rightBound;
	qint64 m_time;
	BoundingMode m_boundingMode;
	QString m_styleTag;
	QStringList m_displayFormatList;
};
