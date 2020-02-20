#include "MCustomSlider.h"

#include <QWheelEvent>

namespace
{
	class Filter : public QObject
	{
		bool eventFilter(QObject* watched, QEvent* event) override
		{
			if(event->type() == QEvent::Wheel)
			{
				QWheelEvent* we = dynamic_cast<QWheelEvent*>(event);
				const int position = m_slider->sliderPosition();
				const int minimum = m_slider->minimum();
				const int maximum = m_slider->maximum();
				const auto delta = we->pixelDelta().y();

				if((position == minimum && delta < 0.0) ||
					(position == maximum && delta > 0.0))
					return true;
			}
			return QObject::eventFilter(watched, event);
		}
	public:
		Filter(QSlider* s) : QObject(s), m_slider(s){}
	private:
		QSlider* m_slider;
	};
}

MCustomSlider::MCustomSlider(QWidget *parent)
	: QSlider(parent)
{
	setFocusPolicy(Qt::StrongFocus);
}

bool MCustomSlider::editableInFocus() const
{
	return m_editableInFocus;
}

void MCustomSlider::setEditableInFocus(bool value)
{
	m_editableInFocus = value;
	if(m_editableInFocus)
		installFilter();
	else
		removeFilter();
}

void MCustomSlider::wheelEvent(QWheelEvent *event)
{
	if(editableInFocus() && !hasFocus())
	{
		event->ignore();
		return;
	}
	QSlider::wheelEvent(event);
}

void MCustomSlider::installFilter()
{
	if(!m_eventFilter)
		m_eventFilter = new Filter(this);
	installEventFilter(m_eventFilter);
}

void MCustomSlider::removeFilter()
{
	if(m_eventFilter)
	{
		removeEventFilter(m_eventFilter);
		m_eventFilter.clear();
	}
}
