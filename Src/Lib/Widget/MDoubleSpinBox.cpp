#include "MDoubleSpinBox.h"

#include <cmath>
#include <QWheelEvent>

MDoubleSpinBox::MDoubleSpinBox(QWidget * parent/* = 0*/)
	: QDoubleSpinBox(parent)
{
	setFocusPolicy(Qt::StrongFocus);
}

QValidator::State MDoubleSpinBox::validate(QString & input, int & pos) const
{
	auto validity = QDoubleSpinBox::validate(input, pos);
	if(fixupWithStep() && validity == QValidator::Acceptable)
	{
		const int intValue = valueFromText(pureText(input)) * pow(10.f, decimals());
		const int stepValue = singleStep() * pow(10.f, decimals());
		if(intValue % stepValue)
			validity = QValidator::Invalid;
	}
	return validity;
}

QString MDoubleSpinBox::pureText(QString const& input) const
{
	return QString(input).remove(suffix()).remove(prefix());
}

void MDoubleSpinBox::wheelEvent(QWheelEvent *event)
{
	if(editableInFocus() && !hasFocus())
	{
		event->ignore();
		return;
	}
	QDoubleSpinBox::wheelEvent(event);
}
