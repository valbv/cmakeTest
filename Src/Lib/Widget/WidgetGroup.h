#pragma once

#include <QList>

#include "MovaviWidgetLib.h"

class QWidget;

class MOVAVIWIDGET_API WidgetGroup
{
public:
	WidgetGroup() = default;
	WidgetGroup(QList<QWidget *> const & widgets);

	void addWidget(QWidget *);
	void addWidgets(QList<QWidget *> const &);
	void removeWidget(QWidget *);

	QList<QWidget *> widgets() const;

	void setVisible(bool);
	void setEnabled(bool);

	void hide();
	void show();
	void enable();
	void disable();

private:
	QList<QWidget *> m_widgets;
};
