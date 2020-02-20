#include "MTimeEdit.h"

#include <algorithm>

MTimeEdit::MTimeEdit( QWidget *parent ) : QTimeEdit(parent)
{
	connect( this , SIGNAL ( dateTimeChanged( QDateTime const & ) )
		   , this , SLOT   ( updateStyle() ) );

	setWrapping(true);
	setFocusPolicy( Qt::StrongFocus );
}

void MTimeEdit::stepBy(int steps)
{
	int stepCost = 0;
	switch( currentSection() )
	{
		case MSecSection:   stepCost = m_msecStep; break;
		case SecondSection: stepCost =       1000; break;
		case MinuteSection: stepCost =    60*1000; break;
		case HourSection:   stepCost =  3600*1000; break;
		default:            stepCost =          0; break;
	}
	QTime changedTime = time().addMSecs( steps * stepCost );

	if (steps < 0 && changedTime > time()) // wraparound "up"
		changedTime = QTime( 0,0,0,0 );

	if (steps > 0 && changedTime < time()) // wraparound "down"
		changedTime = QTime( 23,59,59,999 );

	if (changedTime > maximumTime())
		setTime( maximumTime() );
	else
	if (changedTime < minimumTime())
		setTime( minimumTime() );
	else
		setTime( changedTime );
}

void MTimeEdit::keyPressEvent( QKeyEvent *event )
{
	int     pos = lineEdit()->cursorPosition();
	QString txt = lineEdit()->text();

	QTimeEdit::keyPressEvent(event);

	QString newTxt = lineEdit()->text();
	const int deletedCount = txt.size() - newTxt.size();

	if(deletedCount > 0)
	{
		auto pairResult = std::mismatch(newTxt.begin(), newTxt.end(), txt.begin());
		const int index = std::distance(newTxt.begin(), pairResult.first);

		for(int i = 0; i < deletedCount; ++i)
			newTxt.insert(index, '0');

		setTime(QTime::fromString(newTxt, displayFormat()));
		lineEdit()->setCursorPosition(index);
		return;
	}

	if (txt != lineEdit()->text())
		return;

	QString newChr;
	switch( event->key() )
	{
		case Qt::Key_0: newChr = "0"; break;
		case Qt::Key_1: newChr = "1"; break;
		case Qt::Key_2: newChr = "2"; break;
		case Qt::Key_3: newChr = "3"; break;
		case Qt::Key_4: newChr = "4"; break;
		case Qt::Key_5: newChr = "5"; break;
		case Qt::Key_6: newChr = "6"; break;
		case Qt::Key_7: newChr = "7"; break;
		case Qt::Key_8: newChr = "8"; break;
		case Qt::Key_9: newChr = "9"; break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
			lineEdit()->deselect();
			clearFocus();
			emit EnterPressed();
			return;
		default:
			return;
	}

	if ( pos >= txt.length() )
		return;

	QString oldChr = txt.mid(pos, 1);
	if ( oldChr.contains(QRegExp("\\d")) )
	{
		txt.replace(pos, 1, newChr);

		const QTime newTime = QTime::fromString(txt, displayFormat());

		if(newTime.isValid())
			setTime(newTime);
		else
			return;
	}

	for( pos += 1 ; pos < txt.length() ; pos += 1 ) // go to next digit
	{
		if ( txt.mid(pos, 1).contains(QRegExp("\\d")) )
			break;
	}

	lineEdit()->setCursorPosition( pos );
	lineEdit()->deselect();
}

void MTimeEdit::showEvent( QShowEvent *event )
{
	updateStyle();

	QTimeEdit::showEvent( event );
}

void MTimeEdit::setStyleTag( QString const &str )
{
	m_styleTag = str;

	updateStyle();
}

void MTimeEdit::setDisplayFormatList( QStringList const &list )
{
	m_displayFormatList = list;

	updateStyle();
}

void MTimeEdit::setMSecStep(int msec)
{
	m_msecStep = msec;
}

void MTimeEdit::updateStyle()
{
	if ( m_displayFormatList.size() > 0 )
	{
		QTime matchTime;
		if(hasFocus() || property("forceMaxtimeFormat").toBool())
			matchTime = maximumTime();
		else
			matchTime = time();

		// find format that gives minimal error, or first in case of equal error
		QString selectedFormat = "";
		double  selectedError  = 1e300;

		QStringList::iterator I = m_displayFormatList.begin();
		QStringList::iterator E = m_displayFormatList.end();
		for ( ; I != E ; ++I )
		{
			//QString fmt = *I;

			QString visibleString = matchTime.toString(*I);
			QTime  visibleTime    = QTime::fromString( visibleString , *I );
			double visibleError   = matchTime.msecsTo( visibleTime );

			if (visibleError < 0 )
				visibleError = -visibleError;

			if ( visibleError < selectedError )
			{
				selectedFormat = *I;
				selectedError  = visibleError;
			}
		}

		if ( selectedFormat != displayFormat() )
		{
			int posFromRight = lineEdit()->text().length() - lineEdit()->cursorPosition();
			setDisplayFormat( selectedFormat );

			int newPos = lineEdit()->text().length() - posFromRight;
			lineEdit()->setCursorPosition( newPos );
		}
	}

	if ( m_styleTag == "shrinkWidth" )
	{
		QFontMetrics fontMetrics(font());
		int width = fontMetrics.width( lineEdit()->text() ) + 4;

		setFixedWidth(width);
	}
}

void MTimeEdit::setTime( QTime const &time )
{
	QTimeEdit::setTime(qBound(minimumTime(), time, maximumTime()));
	updateStyle();
}

void MTimeEdit::setMaximumTime( QTime const &time )
{
	QTimeEdit::setMaximumTime( time );
}

void MTimeEdit::focusInEvent( QFocusEvent *event )
{
	setFocusPolicy(Qt::WheelFocus);
	QTimeEdit::focusInEvent(event);
	updateStyle();
}

void MTimeEdit::focusOutEvent( QFocusEvent *event )
{
	setFocusPolicy(Qt::StrongFocus);
	QTimeEdit::focusOutEvent(event);
	updateStyle();
}

void MTimeEdit::wheelEvent(QWheelEvent *event)
{
	if (focusPolicy() == Qt::StrongFocus)
	{
		event->ignore();
	}
	else
	{
		QTimeEdit::wheelEvent(event);
	}
}
