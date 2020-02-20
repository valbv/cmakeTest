#pragma once

#include "AbstractLayoutController.h"

#include "MovaviWidgetLib.h"

/// @brief Фейковый виджет, который позволяет получить доступ из
/// стилей к параметрам Layout'а, ассоциированного с родительским виджетом или c переданным лэйаутом

class MOVAVIWIDGET_API LayoutController : public QWidget, public AbstractLayoutController
{
	Q_OBJECT
	DECLARE_LAYOUT_CONTROLLER_PROPERTIES
public:
	LayoutController(QWidget *);                             // управляет layout-ом виджета
	LayoutController(QLayout *, QWidget * parent = nullptr); // управляет переданным layout-ом
	~LayoutController();

	static LayoutController * getLayoutController(QLayout *);
	static LayoutController * getLayoutController(QWidget *);

	QLayout * getLayout() const override;

protected:
	QPointer<QLayout> m_layout;
	QPointer<QWidget> m_widget;
};