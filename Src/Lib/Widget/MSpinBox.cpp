#include "MSpinBox.h"
#include <QWheelEvent>

MSpinBox::MSpinBox(QWidget * parent/* = 0*/)
	: QSpinBox(parent)
{
	setFocusPolicy(Qt::StrongFocus);
}

void MSpinBox::wheelEvent(QWheelEvent *event)
{
	if(editableInFocus() && !hasFocus())
	{
		event->ignore();
		return;
	}
	QSpinBox::wheelEvent(event);
}
