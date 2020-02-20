#pragma once

#include <QComboBox>

#include "MovaviWidgetLib.h"

/**
 * @brief Класс раскрывающегося списка с дополнительными настройками.
 * Класс позволяет использовать широкие popup-списки, отключать обработку колёсика мыши и обрезать отображаемый текст
 * А, также, позволяет использовать кастомный попап-виджет
 */
class MOVAVIWIDGET_API MComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(bool    mouseWheelEnabled  READ mouseWheelEnabled  WRITE setMouseWheelEnabled  NOTIFY mouseWheelEnabledChanged  DESIGNABLE true)
	Q_PROPERTY(bool    widePopupEnabled   READ widePopupEnabled   WRITE setWidePopupEnabled   NOTIFY widePopupEnabledChanged   DESIGNABLE true)
	Q_PROPERTY(bool    eliding            READ isEliding          WRITE setEliding            NOTIFY elidingChanged            DESIGNABLE true)
	Q_PROPERTY(QString displayText        READ displayText        WRITE setDisplayText        NOTIFY displayTextChanged        DESIGNABLE true)

public:

	explicit MComboBox(QWidget * parent = 0);

	bool mouseWheelEnabled() const;
	void setMouseWheelEnabled(bool enabled);

	bool widePopupEnabled() const;
	void setWidePopupEnabled(bool enabled);

	bool isEliding() const;
	void setEliding(bool set);

	QString displayText() const;
	void setDisplayText(const QString &);

	/// Установить кастомный попап, который будет показываться при клике мышью.
	/// Если popup == 0, будет использоваться стандартный выпадающий список
	/// Комбобокс не берёт на себя управление жизненным циклом виджета
	void setCustomPopup(QWidget * popup);

	void showPopup() override;
	void hidePopup() override;

signals:
	void mouseWheelEnabledChanged();
	void widePopupEnabledChanged();
	void elidingChanged();
	void displayTextChanged();

protected:
	void paintEvent(QPaintEvent * event) override;
	void wheelEvent(QWheelEvent * event) override;
	QSize minimumSizeHint() const override;

private:
	bool     m_mouseWheelEnabled;
	bool     m_widePopupEnabled;
	bool     m_eliding;
	QString  m_displayText;
	QWidget *m_popup;
};
