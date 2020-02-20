#include "MMagnet.h"
#include "MSlider.h"
#ifdef __APPLE__
#include <cstdlib>
#endif

/////////////////////////////
// MMagnetSliderThumbPoint //
/////////////////////////////
int MMagnetSliderThumbPoint::position() const
{
	return m_thumb.value();
}



/////////////
// MMagnet //
/////////////
MMagnet::MMagnet(int initialPosition)
	: m_previousPosition(initialPosition)
{
}

void MMagnet::attachPoint(MMagnetAbstractPoint* point, const Magnetism& magnetism)
{
	m_points[point] = magnetism;
}

void MMagnet::detachPoint(MMagnetAbstractPoint* point)
{
	m_points.remove(point);
}

int MMagnet::moveTo(int newPosition)
{
	typedef QMap<MMagnetAbstractPoint*, Magnetism>::const_iterator iterator;
	for(iterator at(m_points.begin()), end(m_points.end()); at != end; ++at)
	{
		int magnetism = 0;
		int pointPosition = at.key()->position();
		int distanceFromPoint = std::abs(pointPosition - newPosition);

		if(pointPosition > m_previousPosition) // left move
		{
			if(newPosition > m_previousPosition) // move in
				magnetism = at.value().leftMoveIn();
			else if(newPosition < m_previousPosition) // move out
				magnetism = at.value().leftMoveOut();
		}
		else if(pointPosition < m_previousPosition) // right move
		{
			if(newPosition > m_previousPosition) // move out
				magnetism = at.value().rightMoveOut();
			else if(newPosition < m_previousPosition) // move in
				magnetism = at.value().rightMoveIn();
		}
		else // detect move direction
		{
			if(newPosition > m_previousPosition) // right move out
				magnetism = at.value().rightMoveOut();
			else if(newPosition < m_previousPosition) // left move out
				magnetism = at.value().leftMoveOut();
		}

		if(distanceFromPoint <= magnetism)
		{
			// найдена точка, к которой надо примагнититься
			newPosition = pointPosition;
			break;
		}
	}

	m_previousPosition = newPosition;
	return newPosition;
}
