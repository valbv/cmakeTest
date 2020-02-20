#include "MStackedWidget.h"

MStackedWidget::MStackedWidget(QWidget * parent) : QStackedWidget(parent)
{
	connect(this, &QStackedWidget::currentChanged, this, &MStackedWidget::fitToContent);
}

void MStackedWidget::setAutoFit(bool fit)
{
	m_autoFit = fit;
	fitToContent();
}

void MStackedWidget::showEvent(QShowEvent * e)
{
	fitToContent();
	QStackedWidget::showEvent(e);
}

QSize MStackedWidget::sizeHint() const
{
	return m_autoFit && currentWidget()
		? currentWidget()->minimumSizeHint() // use current widget MINIMUM geometry
		: QStackedWidget::sizeHint();
}

QSize MStackedWidget::minimumSizeHint() const
{
	return m_autoFit && currentWidget()
		? currentWidget()->minimumSizeHint() // use current widget geometry
		: QStackedWidget::minimumSizeHint();
}

void MStackedWidget::fitToContent()
{
	if (!m_autoFit)
		return;
	updateGeometry();
}
