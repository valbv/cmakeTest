#pragma once

#include <QWidget>
#include <QTimeEdit>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>
#include <QStringList>

#include "MovaviWidgetLib.h"

class MOVAVIWIDGET_API MTimeEdit : public QTimeEdit
{
	Q_OBJECT

	Q_PROPERTY(QString		styleTag			READ styleTag			WRITE setStyleTag)
	Q_PROPERTY(QStringList	displayFormatList	READ displayFormatList	WRITE setDisplayFormatList)

public:
	MTimeEdit( QWidget *parent );

	QString     styleTag()          { return m_styleTag; }
	QStringList displayFormatList() { return m_displayFormatList; }

	void setStyleTag          ( QString const & );
	void setDisplayFormatList ( QStringList const & );
	void setTime( QTime const &time );
	void setMaximumTime(QTime const &time);
	void setMSecStep(int mscec);

	void focusInEvent( QFocusEvent *event );
	void focusOutEvent( QFocusEvent *event );
	virtual void wheelEvent(QWheelEvent *event);
	virtual void stepBy(int steps);
	virtual void keyPressEvent( QKeyEvent *event );
	virtual void showEvent( QShowEvent *event );
signals:
	void EnterPressed();
public slots:
	void updateStyle();
private:
	int m_msecStep = 1;
	QString m_styleTag;
	QStringList m_displayFormatList;
};
