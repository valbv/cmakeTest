#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QRect>

#include "MLineEdit.h"
#include "Unicode.h"

//..............................................................................

MLineEdit::MLineEdit(QWidget *parent) : QLineEdit(parent),
	m_eliding(true),
	m_focused(false)
{}

//..............................................................................

MLineEdit::~MLineEdit()
{}

//..............................................................................

void MLineEdit::setEliding(bool eliding)
{
	if (eliding == m_eliding)
		return;
	m_eliding = eliding;
}

bool MLineEdit::isEliding() const
{
	return m_eliding;
}

void MLineEdit::setOriginalText(const QString &str)
{
	if (originalText() == str)
		return;

	m_originalText = str;
	// Коррекция направления текста
	m_originalText.prepend(LEFT_TO_RIGHT_EMBEDDING);
	m_originalText.append(POP_DIRECTIONAL_FORMATTING);
	updateViewText();

	emit originalTextChanged(str);
}

//..............................................................................

QString MLineEdit::originalText() const
{
	QString result(m_originalText);
	if (!result.isEmpty() && result.at(0) == LEFT_TO_RIGHT_EMBEDDING)
		result = result.remove(LEFT_TO_RIGHT_EMBEDDING).remove(POP_DIRECTIONAL_FORMATTING);

	return result;
}

//..............................................................................

void MLineEdit::resizeEvent(QResizeEvent * event)
{
	updateViewText();
	QLineEdit::resizeEvent(event);
}

//..............................................................................

void MLineEdit::focusInEvent( QFocusEvent *event )
{
	m_focused = true;
	updateViewText();
	QLineEdit::focusInEvent(event);
}

//..............................................................................

void MLineEdit::focusOutEvent( QFocusEvent *event )
{
	m_focused = false;

	auto m_currentText = text();
	setOriginalText(m_currentText);

	QLineEdit::focusOutEvent(event);
}

//..............................................................................

void MLineEdit::updateViewText()
{
	if (!m_focused && m_eliding)
	{
		QFontMetrics fontMetrics(font());
		m_viewText = fontMetrics.elidedText(m_originalText, Qt::ElideRight, width());
	}
	else
		m_viewText = m_originalText;

	setText(m_viewText);

	if (m_viewText != m_originalText)
		setToolTip(m_originalText);
	else
		setToolTip(QString());
	setCursorPosition(0);
}
