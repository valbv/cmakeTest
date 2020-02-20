#pragma once
#include <QDoubleSpinBox>

#include "MovaviWidgetLib.h"

/**
 * @brief MDoubleSpinBox добавляет валидацию для QDoubleSpinBox на вводимые значения, ибо QDoubleSpinBox позволяет вводить значения не кратные заданному шагу
 * добавляет опцию editableInFocus которая позволяет редактировать значение спинбокса только если тот находится в фокусе
 */

class MOVAVIWIDGET_API MDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
	///@brief по умолчанию при ручном вводе значения не учитывается singleStep(), данная опция позволяет делать это
	Q_PROPERTY(bool fixupWithStep   READ fixupWithStep   WRITE setFixupWithStep   DESIGNABLE true)
	Q_PROPERTY(bool editableInFocus READ editableInFocus WRITE setEditableInFocus DESIGNABLE true)
public:
	MDoubleSpinBox(QWidget * = 0);
	bool fixupWithStep() const { return m_fixupWithStep; }
	void setFixupWithStep(bool v){ m_fixupWithStep = v; }
	bool editableInFocus() const { return m_editableInFocus; }
	void setEditableInFocus(bool v){ m_editableInFocus = v; }
protected:
	QValidator::State validate(QString &, int &) const override;
	QString pureText(const QString&) const;
	void wheelEvent(QWheelEvent *) override;

	bool m_fixupWithStep   = false;
	bool m_editableInFocus = true;
};
