#pragma once

#include <QFrame>
#include <QLayout>

#include "AbstractLayoutController.h"

#include "MovaviWidgetLib.h"

/// @brief Небольшая надстройка над QFrame, которая позволяет получить доступ из
/// стилей к параметрам Layout'а, ассоциированного с этим фреймом.
class MOVAVIWIDGET_API MFrame : public QFrame, public AbstractLayoutController
{
	Q_OBJECT
	DECLARE_LAYOUT_CONTROLLER_PROPERTIES
public:
	MFrame(QWidget *parent = nullptr, Qt::WindowFlags flags = {});

protected:
	QLayout * getLayout() const override;
};
