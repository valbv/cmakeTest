#pragma once

#include <QStackedWidget>

#include "MovaviWidgetLib.h"

/**
 * @brief MStackedWidget обладает свойством подстраиваться под размер содержимого.
 */
class MOVAVIWIDGET_API MStackedWidget : public QStackedWidget
{
	Q_OBJECT
	Q_PROPERTY(bool autoFit READ autoFit WRITE setAutoFit DESIGNABLE true)

public:
	MStackedWidget(QWidget *parent = 0);

public:
	bool autoFit() const { return m_autoFit; }
	void setAutoFit(bool autoFit);

protected:
	void showEvent(QShowEvent *) override;
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

public slots:
	void fitToContent();

private:
	bool m_autoFit = true;
};
