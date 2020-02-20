#pragma once

#include <QMap>

#include "MovaviWidgetLib.h"

class MSliderThumb;
class MMagnetAbstractPoint;
class MMagnetFixedPoint;
class MMagnetSliderThumbPoint;
class MMagnet;

/// @class MMagnetAbstractPoint
/// @brief Абстрактный класс точки примагничивания
class MOVAVIWIDGET_API MMagnetAbstractPoint
{
public:
	virtual ~MMagnetAbstractPoint() {}

	/// @brief Функция возвращает позиции точки к которой надо примагнититься
	virtual int position() const = 0;
};

/// @class MMagnetFixedPoint
/// @brief Класс возвращающий фиксированную точку примагничивания
class MOVAVIWIDGET_API MMagnetFixedPoint : public MMagnetAbstractPoint
{
public:
	explicit MMagnetFixedPoint(int position = 0) : m_position(position) {}

	/// @brief Возвращает фиксированную точку примагничивания
	int position() const override { return m_position; }

private:
	int m_position;

};

/// @class MMagnetSliderThumbPoint
/// @brief Класс возвращающий в качестве точки примагничивания позицию thumb'а
class MOVAVIWIDGET_API MMagnetSliderThumbPoint : public MMagnetAbstractPoint
{
public:
	explicit MMagnetSliderThumbPoint(MSliderThumb& thumb) : m_thumb(thumb) {}

	/// @brief Возвращает позицию (value) MSliderThumb переданного в конструкторе
	int position() const override;

private:
	MSliderThumb& m_thumb;
};


/// @class MMagent
/// @brief Класс помогающий реализовать функционал "примагничивания"
/// @details К объектам класса можно добавлять "точки примагничивания" (MMagnetAbstractPoint)
/// указывая границы притяжения/примагничивания (подробнее в описании класса Magnetism).
/// Функция moveTo(int newPosition) возвращает значение позиции с учётом примагничивания
/// @code
/// MMagnet magnet;
/// MMagnetFixedPoint point1(30), point2(100);
/// magnet.attachPoint(&point1, MMagnet::Magnetism(10));
/// magnet.attachPoint(&point2, MMagnet::Magnetism(40, 40, 0, 0));
/// magnet.moveTo(15); // returns 15 не попадает в зону действия точек примагничивания
/// magnet.moveTo(25); // returns 30 попадает в зону действия первой точки (left, moveIn)
/// magnet.moveTo(35); // returns 30 попадает в зону действия первой точки (right, moveOut)
/// magnet.moveTo(55); // returns 55 не попадает в зону действия точек примагничивания
/// magnet.moveTo(75); // returns 100 попадает в зону действия второй точки (left, moveIn)
/// magnet.moveTo(105);// returns 105 не попадает в зону действия точек примагничивания
/// @endcode
/// @todo При желании можно написать юнит-тест, ради юнит-теста
class MOVAVIWIDGET_API MMagnet
{
public:
	/// @class MMagent::Magnetism
	/// @brief Класс определяющий зону примагничивания
	/// @details Определяет четыре зоны примагничивания:<ol>
	/// <li> слева от точки, при приблежении к ней leftMoveIn
	/// <li> слева от точки, при удалении от неё leftMoveOut
	/// <li> справа от точки, при приблежении к ней rightMoveIn
	/// <li> справа от точки, при удалении от неё rightMoveOut</ol>
	/// Слева - это значение point->position() < предыдущее полжение магнита
	/// Справа - это значение point->position() > предыдущее полжение магнита
	class Magnetism
	{
	public:
		/// @brief Конструктор устанавливаюший все зоны приманичивания в значение @a distance
		explicit Magnetism(int distance = 0)				{ setDistance(distance); }

		/// @brief Конструктор устанавливающий индивидуальные значения зон примагничивания
		Magnetism(int leftMoveIn, int leftMoveOut, int rightMoveIn, int rightMoveOut)
		{
			setDistance(leftMoveIn, leftMoveOut, rightMoveIn, rightMoveOut);
		}

		/// @name геттеры зон примагничивания
		/// @{
		int leftMoveIn() const throw()					{ return m_leftMoveIn; }
		int leftMoveOut() const throw()					{ return m_leftMoveOut; }
		int rightMoveIn() const throw()					{ return m_rightMoveIn; }
		int rightMoveOut() const throw()				{ return m_rightMoveOut; }
		/// @}

		/// @name сеттеры зон примагничивания
		/// @{
		void setLeftMoveIn(int distance) throw()		{ m_leftMoveIn = distance; }
		void setLeftMoveOut(int distance) throw()		{ m_leftMoveOut = distance; }
		void setRightMoveIn(int distance) throw()		{ m_rightMoveIn = distance; }
		void setRightMoveOut(int distance) throw()		{ m_rightMoveOut = distance; }

		void setLeftMove(int distance) throw()			{ setLeftMove(distance, distance); }
		void setLeftMove(int moveInDistance, int moveOutDistance) throw()
		{
			setLeftMoveIn(moveInDistance);
			setLeftMoveOut(moveOutDistance);
		}

		void setRightMove(int distance) throw()			{ setRightMove(distance, distance); }
		void setRightMove(int moveInDistance, int moveOutDistance) throw()
		{
			setRightMoveIn(moveInDistance);
			setRightMoveOut(moveOutDistance);
		}

		void setMoveIn(int distance) throw()			{ setMoveIn(distance, distance); }
		void setMoveIn(int leftDistance, int rightDistance) throw()
		{
			setLeftMoveIn(leftDistance);
			setRightMoveIn(rightDistance);
		}

		void setMoveOut(int distance) throw()			{ setMoveOut(distance, distance); }
		void setMoveOut(int leftDistance, int rightDistance) throw()
		{
			setLeftMoveOut(leftDistance);
			setRightMoveOut(rightDistance);
		}

		void setDistance(int distance) throw()			{ setLeftMove(distance); setRightMove(distance); }
		void setDistance(int leftMoveIn, int leftMoveOut, int rightMoveIn, int rightMoveOut) throw()
		{
			setLeftMove(leftMoveIn, leftMoveOut);
			setRightMove(rightMoveIn, rightMoveOut);
		}
		/// @}

	private:
		int m_leftMoveIn;
		int m_leftMoveOut;
		int m_rightMoveIn;
		int m_rightMoveOut;
	};

	/// @brief Конструктор. @a initialPosition - позиция магнита
	explicit MMagnet(int initialPosition);

	/// @brief Подключает точку примагничивания @a point с зонами магнитизма @a magnetism
	/// @note MMagnet не управляет временем жизни точек примагничивания. Это обязанности клиентского кода.
	void attachPoint(MMagnetAbstractPoint* point, const Magnetism& magnetism);

	/// @brief Отключает точку примагничивания @a point
	void detachPoint(MMagnetAbstractPoint* point);

	/// @brief Функция перемещает магнит в позицию @a newPosition
	/// @returns Возвращает позицию магнита с учётом примагничивания
	int moveTo(int newPosition);

private:
	QMap<MMagnetAbstractPoint*, Magnetism>	m_points;
	int m_previousPosition;
};
