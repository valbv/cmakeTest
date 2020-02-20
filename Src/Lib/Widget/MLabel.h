#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QResizeEvent>

#include "MovaviWidgetLib.h"

/**
 * @brief MLabel позволяет обрезать "лишний" текст и обрабатывать клики.
 */
class MOVAVIWIDGET_API MLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString				originalText	READ	originalText		WRITE	setOriginalText		NOTIFY	originalTextChanged	DESIGNABLE true)
	Q_PROPERTY(bool					eliding			READ	isEliding			WRITE	setEliding										DESIGNABLE true)
	Q_PROPERTY(Qt::TextElideMode	elideMode		READ	elideMode			WRITE	setElideMode									DESIGNABLE true)
	Q_PROPERTY(bool					clickable		READ	isClickable			WRITE	setClickable									DESIGNABLE true)
	Q_PROPERTY(bool					directionFix	READ	hasDirectionFix		WRITE	setDirectionFix									DESIGNABLE true)

public:
	static const std::string CLICKABLE;

public:
	MLabel(QWidget *parent = 0);

public:
	void setOriginalText(const QString &str);
	QString originalText() const;
	bool isEliding() const { return m_eliding; }
	void setEliding(bool eliding);
	void setElideMode(Qt::TextElideMode mode) { m_elideMode = mode; }
	Qt::TextElideMode elideMode() const { return m_elideMode; }
	bool isClickable() const { return m_clickable; }
	void setClickable(bool clickable);
	bool hasDirectionFix() const { return m_directionFix; }
	void setDirectionFix(bool directionFix);

protected:
	void mousePressEvent(QMouseEvent * event);
	void resizeEvent(QResizeEvent *event);
	void updateViewText();

signals:
	void clicked();
	void originalTextChanged(const QString &text);

private:
	QString m_originalText;
	QString m_viewText;
	bool m_eliding;
	bool m_clickable;
	bool m_directionFix;
	Qt::TextElideMode m_elideMode = Qt::ElideRight;
};
