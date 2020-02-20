#pragma once
#include <QSlider>
#include <QPointer>

#include "MovaviWidgetLib.h"

/**
 * @brief MCustomSlider реализует следующее поведение при включенной опции editableInFocus
 * слайдер возможно редактировать при том лишь условии что слайдер находится в фокусе
 * попадание фокуса на виджет регулируется настройками FocusPolicy
 */

class MOVAVIWIDGET_API MCustomSlider : public QSlider
{
	Q_OBJECT
	Q_PROPERTY(bool editableInFocus READ editableInFocus WRITE setEditableInFocus DESIGNABLE true)
public:
	MCustomSlider(QWidget * = 0);
	bool editableInFocus() const;
	void setEditableInFocus(bool);
protected:
	void wheelEvent(QWheelEvent *) override;
private:
	void installFilter();
	void removeFilter();
private:
	bool m_editableInFocus = true;
	QPointer<QObject> m_eventFilter;
};
