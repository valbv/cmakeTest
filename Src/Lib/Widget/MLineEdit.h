#pragma once

#include <QLineEdit>
#include <QWheelEvent>

#include "MovaviWidgetLib.h"

/**
@brief LineEdit с отображением ... при обрезании текста
*/
class MOVAVIWIDGET_API MLineEdit : public QLineEdit
{
	Q_OBJECT
	Q_PROPERTY(bool		eliding			READ	isEliding		WRITE	setEliding									DESIGNABLE true)
	Q_PROPERTY(QString	originalText	READ	originalText	WRITE	setOriginalText	NOTIFY	originalTextChanged	DESIGNABLE true)

public:
	MLineEdit(QWidget *parent);
	~MLineEdit();

public:
	bool isEliding() const;
	void setEliding(bool);

	QString originalText() const;
	void setOriginalText(const QString &);

protected:
	void resizeEvent(QResizeEvent * event);

	void focusInEvent(QFocusEvent * event);
	void focusOutEvent(QFocusEvent * event);

	void updateViewText();

signals:
	void originalTextChanged(const QString & text);

private:
	QString m_originalText;
	QString m_viewText;
	bool m_eliding;
	bool m_focused;
};
