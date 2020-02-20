#include "LayoutSupressor.h"

#include <QLayout>

LayoutSupressor::LayoutSupressor(QLayout * layout)
	: m_layout(layout)
{
	m_layout->setEnabled(false);
}
LayoutSupressor::~LayoutSupressor()
{
	m_layout->setEnabled(true);
	m_layout->activate();
}
