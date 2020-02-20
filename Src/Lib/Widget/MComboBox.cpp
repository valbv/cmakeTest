#include <QAbstractItemView>
#include <QScrollBar>
#include <QEvent>
#include <QWheelEvent>
#include <QStylePainter>
#include <QListView>
#include <QTimer>
#include <QPaintDevice>

#include "MComboBox.h"

class AdvancedComboView : public QListView
{
public:
	AdvancedComboView(QComboBox *cmb, bool wide) : combo(cmb), isWide(wide)
	{
		if (isWide)
			setTextElideMode(Qt::ElideNone);
	}

	void changeEvent(QEvent * event)
	{
		switch (event->type())
		{
			case QEvent::FontChange:
			case QEvent::StyleChange:
				struct Helper : public QObject
				{
					QListView *m_view;

					Helper(QListView *v) : m_view(v) { setParent(v); }

					~Helper()
					{
						m_view->setFlow(QListView::TopToBottom);
					}
				};

				QTimer::singleShot(0, new Helper(this), SLOT(deleteLater()));
				break;
			case QEvent::LanguageChange:
				if(combo->view()->isVisible())
				{
					///@NOTE размеры вьюхи для комбобокса пересчитываются в момент вызова showPopup()
					combo->hidePopup();
					combo->showPopup();
				}
				break;
			default:
				break;
		}

		QListView::changeEvent(event);
	}

protected:
	void resizeEvent(QResizeEvent *event)
	{
		resizeContents(viewport()->width(), contentsSize().height());
		QListView::resizeEvent(event);

		if (isWide)
		{
			QScrollBar *vertScroll = verticalScrollBar();
			int scrollWidth = vertScroll && vertScroll->maximum() ? vertScroll->width() : 0;

			// Берем желаемую ширину столбца
			int columnWidthHint = sizeHintForColumn(combo->modelColumn());
			setMinimumWidth(columnWidthHint + scrollWidth);
		}
	}

	QStyleOptionViewItem viewOptions() const
	{
		QStyleOptionViewItem option = QListView::viewOptions();
		option.showDecorationSelected = true;
		if (combo)
			option.font = combo->font();
		return option;
	}

	void paintEvent(QPaintEvent *e)
	{
		if (combo) {
			QStyleOptionComboBox opt;
			opt.initFrom(combo);
			opt.editable = combo->isEditable();
			if (combo->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, combo)) {
				//we paint the empty menu area to avoid having blank space that can happen when scrolling
				QStyleOptionMenuItem menuOpt;
				menuOpt.initFrom(this);
				menuOpt.palette = palette();
				menuOpt.state = QStyle::State_None;
				menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
				menuOpt.menuRect = e->rect();
				menuOpt.maxIconWidth = 0;
				menuOpt.tabWidth = 0;
				QPainter p(viewport());
				combo->style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
			}
		}
		QListView::paintEvent(e);
	}

private:
	QComboBox *combo;

	bool isWide;
};


MComboBox::MComboBox(QWidget * parent) : QComboBox(parent),
	 m_mouseWheelEnabled(true),
	 m_widePopupEnabled(true),
	 m_eliding(true),
	 m_popup(0)
{
	setFocusPolicy(Qt::StrongFocus);

	setView(new AdvancedComboView(this, m_widePopupEnabled));
}

 bool MComboBox::mouseWheelEnabled() const
 {
	 return m_mouseWheelEnabled;
 }

void MComboBox::setMouseWheelEnabled(bool enabled)
{
	if (m_mouseWheelEnabled == enabled)
		return;

	m_mouseWheelEnabled = enabled;
	emit mouseWheelEnabledChanged();
}

bool MComboBox::widePopupEnabled() const
{
	return m_widePopupEnabled;
}

void MComboBox::setWidePopupEnabled(bool enabled)
{
	if (m_widePopupEnabled == enabled)
		return;
	m_widePopupEnabled = enabled;

	view()->deleteLater();
	setView(new AdvancedComboView(this, m_widePopupEnabled));

	emit widePopupEnabledChanged();
}

bool MComboBox::isEliding() const
{
	return m_eliding;
}

void MComboBox::setEliding(bool set)
{
	if (m_eliding == set)
		return;

	m_eliding = set;
	emit elidingChanged();
}

QString MComboBox::displayText() const
{
	return m_displayText;
}

void MComboBox::setDisplayText(const QString & displayText)
{
	if (m_displayText == displayText)
		return;
	m_displayText = displayText;
	emit displayTextChanged();
	update();
}

void MComboBox::setCustomPopup(QWidget * popup)
{
	if (m_popup == popup)
		return;
	m_popup = popup;
}

void MComboBox::showPopup()
{
	if (!m_popup)
		return QComboBox::showPopup();
	m_popup->show();

	auto pos = mapToGlobal(QPoint(0, 0));
	if (pos.y() > m_popup->height())
		m_popup->move(pos.x(), pos.y() - m_popup->height());
	else
		m_popup->move(pos.x(), pos.y() + height());
}

void MComboBox::hidePopup()
{
	if (!m_popup)
		return QComboBox::hidePopup();

	m_popup->hide();
}

void MComboBox::wheelEvent(QWheelEvent * event)
{
	if ( ! m_mouseWheelEnabled)
	{
		event->setAccepted(false);
		return;
	}

	QComboBox::wheelEvent(event);
}

QSize MComboBox::minimumSizeHint() const
{
	QSize sizeHint = QComboBox::minimumSizeHint();
	if (m_displayText.isEmpty())
		return sizeHint;

	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	QRect textRect = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
	sizeHint.rwidth() = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxFrame, this).width();

	QFontMetrics fontMetrics(font());
	sizeHint.rwidth() -= textRect.width();
	int indent = 0;
#ifdef Q_OS_MAC
	/// @note воркэраунд на маке требуется отступ ибо qstyle ведет себя не корректно
	indent = 10;
#endif
	sizeHint.rwidth() += fontMetrics.width(m_displayText) + indent;
	return sizeHint;
}

void MComboBox::paintEvent(QPaintEvent * event)
{
	if (!m_eliding && m_displayText.isEmpty())
		return QComboBox::paintEvent(event);

	event->accept();

	QStylePainter painter(this);
	QStyleOptionComboBox opt;
	initStyleOption(&opt);

	// рисуем сам комбобокс
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);

	// обрезаем текст в комбобоксе и рисуем надпись
	QString text;
	if (m_displayText.isEmpty())
	{
		QRect textRect = painter.style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
		QFontMetrics fontMetrics(font(), this);
		QString originalText = currentText();
		text = fontMetrics.elidedText(opt.currentText, Qt::ElideRight, textRect.width());

		if (originalText != opt.currentText)
			setToolTip(originalText);
		else
			setToolTip(QString());
	}
	else
		text = m_displayText;

	opt.currentText = text;
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
