#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QRect>

#include "MLabel.h"
#include "Unicode.h"

const std::string MLabel::CLICKABLE("clickable");

//..............................................................................

MLabel::MLabel(QWidget *parent/* = 0*/) : QLabel(parent)
, m_originalText("MLabel")
, m_viewText("MLabel")
, m_eliding(true)
, m_clickable(true)
, m_directionFix(true)
{ setText(m_viewText); }

//..............................................................................

void MLabel::setOriginalText(const QString &str)
{
	if (originalText() == str)
		return;

	m_originalText = str;
	// Коррекция направления текста
	if (m_directionFix)
	{
		m_originalText.prepend(LEFT_TO_RIGHT_EMBEDDING);
		m_originalText.append(POP_DIRECTIONAL_FORMATTING);
	}
	updateViewText();

	emit originalTextChanged(str);
}

//..............................................................................

QString MLabel::originalText() const
{
	QString result(m_originalText);
	if (m_directionFix && !result.isEmpty() && result.at(0) == LEFT_TO_RIGHT_EMBEDDING)
		result = result.remove(LEFT_TO_RIGHT_EMBEDDING).remove(POP_DIRECTIONAL_FORMATTING);

	return result;
}

//..............................................................................

void MLabel::setEliding(bool eliding)
{
	if (m_eliding == eliding)
		return;

	m_eliding = eliding;
	updateViewText();
}

//..............................................................................

void MLabel::setClickable(bool clickable)
{
	if (clickable == m_clickable)
		return;

	m_clickable = clickable;
}

//..............................................................................

void MLabel::setDirectionFix(bool directionFix)
{
	if (directionFix == m_directionFix)
		return;

	const QString str = originalText();
	m_directionFix = directionFix;
	setOriginalText(str);
}

//..............................................................................

void MLabel::mousePressEvent(QMouseEvent *event)
{
	if (m_clickable)
		emit clicked();

	QLabel::mousePressEvent(event);
}

//..............................................................................

void MLabel::resizeEvent(QResizeEvent *event)
{
	updateViewText();

	QLabel::resizeEvent(event);
}

//..............................................................................

void MLabel::updateViewText()
{
	if (!m_eliding)
	{
		setText(m_originalText);
		setToolTip(QString());
	}
	else
	{
		QFontMetrics fontMetrics(font());
		m_viewText = fontMetrics.elidedText(m_originalText, m_elideMode, width()-frameWidth()*2);
		setText(m_viewText);

		if (m_viewText != m_originalText)
			setToolTip(m_originalText);
		else
			setToolTip(QString());
	}
}
