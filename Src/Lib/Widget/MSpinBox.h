#pragma once
#include <QSpinBox>

#include "MovaviWidgetLib.h"

/**
 * @brief MSpinBox добавляет опцию editableInFocus которая позволяет редактировать значение спинбокса только если тот находится в фокусе
 */

class MOVAVIWIDGET_API MSpinBox : public QSpinBox
{
	Q_OBJECT
	Q_PROPERTY(bool editableInFocus READ editableInFocus WRITE setEditableInFocus DESIGNABLE true)
public:
	MSpinBox(QWidget * = 0);
	bool editableInFocus() const { return m_editableInFocus; }
	void setEditableInFocus(bool v){ m_editableInFocus = v; }
protected:
	void wheelEvent(QWheelEvent *) override;

	bool m_editableInFocus = true;
};
