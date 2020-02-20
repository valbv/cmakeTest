#include "MFrame.h"

MFrame::MFrame(QWidget *parent, Qt::WindowFlags flags)
	: QFrame(parent, flags)
{}

QLayout * MFrame::getLayout() const
{
	return layout();
}
