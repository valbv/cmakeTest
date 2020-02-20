#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QToolButton>

#include <math.h>

#include "MSlider.h"
#include "MMagnet.h"

#include "WidgetUtils.h"

MSliderThumb::MSliderThumb(MSlider *parent)
: QToolButton(parent)
, m_parent(parent)
, m_magnet(nullptr)
, m_pressedAt(QPoint(-1,-1))
, m_isPressed(false)
, m_isMoved(false)
, m_value(parent->minimum())
, m_maximum(INT_MAX)
, m_minimum(INT_MIN)
, m_zOrder(0)
, m_pointOffset(7)
, m_parentMargin(0)
, m_isSelectable(true)
, m_isDraggable(true)
, m_isPositionable(true)
, m_isMovableByMouseWheel(true)
, m_isMovableByKeyboard(true)
{
	this->setMouseTracking(true);
	this->setFixedHeight(14);
	this->setFixedWidth(14);

	setCheckable(true);
}

MSliderThumb::~MSliderThumb()
{
	delete m_magnet;
}

void MSliderThumb::repolish() { style()->unpolish(this); style()->polish(this); }
void MSliderThumb::relayout() { m_parent->updateThumbLayout(); }

void MSliderThumb::setMinimum(int min) { m_minimum = min; setValue( m_value ); }

void MSliderThumb::setMaximum(int max) { m_maximum = max; setValue( m_value ); }

void MSliderThumb::setValue(int value)
{
	if (value < m_parent->minimum())
		value = m_parent->minimum();
	if (value > m_parent->maximum())
		value = m_parent->maximum();
	if (value < minimum())
		value = minimum();
	if (value > maximum())
		value = maximum();

	bool changed = (m_value != value);
	m_value = value;
	m_parent->moveThumbTo(this, m_value);
	if (changed)
		emit valueChanged(m_value);
}

void MSliderThumb::mousePressEvent(QMouseEvent *event)
{
	setUpdatesEnabled(false); // avoid flicker
	{
		bool flag = isChecked();

		blockSignals(true);
			setCheckable(false);
		blockSignals(false);

		if (m_isDraggable)
		{
			m_isPressed = true;
			m_pressedAt = event->pos();
		}

		QToolButton::mousePressEvent(event);

		blockSignals(true);
			setCheckable(true);
			setChecked(flag);
		blockSignals(false);
	}
	setUpdatesEnabled(true);
}

void MSliderThumb::mouseReleaseEvent(QMouseEvent *event)
{
	setUpdatesEnabled(false); // avoid flicker
	{
		m_pressedAt = QPoint(-1,-1);

		bool flag = isChecked();

		blockSignals(true);
			setCheckable(false);
		blockSignals(false);

		QToolButton::mouseReleaseEvent(event);

		blockSignals(true);
			setCheckable(true);
			setChecked(flag);
		blockSignals(false);

		if (!m_isMoved && m_isSelectable) // toggle checked state
			setChecked( !isChecked() );

		if (m_isMoved)
			emit dragFinished();

		m_isMoved = false;
		m_isPressed = false;
	}
	setUpdatesEnabled(true);
}

void MSliderThumb::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPressed)
	{
		if (!m_isMoved)
			emit dragStarted();
		m_isMoved = true;

		QPoint pt = mapToParent( event->pos() );
		pt.setX( pt.x() - m_pressedAt.x() + pointOffset() );
		pt.setY( pt.y() - m_pressedAt.y() + pointOffset() );
		int newValue = m_parent->pointToValue(pt);
		if(m_magnet)
			newValue = m_magnet->moveTo(newValue);
		newValue = m_parent->correctStickValue(this, newValue);
		setValue( newValue );

		// оставляем статус pressed, даже если мышь находится не на ручке
		event->setLocalPos(QPoint());

		QToolButton::mouseMoveEvent(event);
		event->setAccepted(true);
	}
	else
	{
		event->setAccepted(false);//передаем ивент на обработку родителю, дабы тот отобразил тултип
	}
}
void MSliderThumb::wheelEvent(QWheelEvent *event)
{
	if(isChecked() && m_isMovableByMouseWheel)
	{
		emit stepped(event->delta());
		event->accept();
	}
}
void MSliderThumb::keyPressEvent(QKeyEvent *event)
{
	if(m_isMovableByKeyboard)
	{
		if(event->key() == Qt::Key_Up || event->key() == Qt::Key_Right)
		{
			emit stepped(WHEEL_STEP);
			event->accept();
			return;
		}
		if(event->key() == Qt::Key_Down || event->key() == Qt::Key_Left)
		{
			emit stepped(-WHEEL_STEP);
			event->accept();
			return;
		}
	}

	event->ignore();
	QToolButton::keyPressEvent(event);
}

MMagnet& MSliderThumb::magnet()
{
	if(!m_magnet)
		m_magnet = new MMagnet(this->value());
	return *m_magnet;
}

void MSliderThumb::moveEvent(QMoveEvent *event)
{
	Util::resetHoverState(this);
	QToolButton::moveEvent(event);
}

void MSlider::wheelEvent(QWheelEvent *event)
{
	if(m_selectedThumb)
		m_selectedThumb->wheelEvent(event);
}
void MSlider::keyPressEvent(QKeyEvent *event)
{
	if(m_selectedThumb)
		m_selectedThumb->keyPressEvent(event);
	else
		QSlider::keyPressEvent(event);
}

void MSlider::changeEvent(QEvent *event)
{
	QSlider::changeEvent(event);
	switch (event->type())
	{
	case QEvent::EnabledChange:
		// Enabling state changed: update background cache pixmap
		updateBackground();
		break;
	default:
		break;
	}
}

MSlider::MSlider(QWidget *parent)
: QSlider(Qt::Vertical, parent)
, m_paintHelper(new QSlider(parent))
, m_selectedThumb(NULL)
, m_defaultThumb(NULL)
, m_isPressed(false)
, m_recursionGuard(false)
, m_isBackgroundChanged(true)
, m_grooveOffset(6)
, m_stickToDefSlider(false)
, m_stickDistance(20)
{
	m_paintHelper->hide();

	m_paintHelper->setStyleSheet(
		" QSlider::handle:horizontal { image:none; border:none; background:none; } "
		" QSlider::handle:vertical   { image:none; border:none; background:none; } "
	);

	setStyleSheet(
		" MSlider                      { background:rgba(0,0,0,0); border:none;                    } "
		" MSlider::handle              { background:rgba(0,0,0,0); border:none;        image:none; } "
		" MSlider::groove              { background:rgba(0,0,0,0); border:none; border-image:none; } "
		" MSlider::add-page            { background:rgba(0,0,0,0); border:none; border-image:none; } "
		" MSlider::sub-page            { background:rgba(0,0,0,0); border:none; border-image:none; } "
		" MSlider::add-page:horizontal { background:rgba(0,0,0,0); border:none; border-image:none; } "
		" MSlider::sub-page:horizontal { background:rgba(0,0,0,0); border:none; border-image:none; } "
		" MSlider::add-page:vertical   { background:rgba(0,0,0,0); border:none; border-image:none; } "
		" MSlider::sub-page:vertical   { background:rgba(0,0,0,0); border:none; border-image:none; } "
	);

	addThumb("default");
}

int MSlider::pointToValue(QPoint point)
{
	double offsetPixels, totalPixels;
	if ( orientation() == Qt::Horizontal )
	{
		offsetPixels = point.x() - (grooveOffset());
		totalPixels = width() -  2*(grooveOffset());
	}
	else
	{
		offsetPixels = point.y() - (grooveOffset());
		totalPixels = height() - 2*(grooveOffset());
	}

	if (totalPixels < 1)
		totalPixels = 1;
	if (offsetPixels < 0)
		offsetPixels = 0;
	if (offsetPixels > totalPixels)
		offsetPixels = totalPixels;

	return minimum() + (maximum() - minimum()) * (offsetPixels / totalPixels);
}

void MSlider::moveThumbTo(MSliderThumb *thumb, int value)
{
	double range = maximum() - minimum();
	if (range == 0)
		range = 1;

	if ( orientation() == Qt::Horizontal )
	{
		int valueOffset = static_cast<int>(((width() - 2.0 * grooveOffset()) * value / range) + 0.5);
		thumb->move( (grooveOffset()) + valueOffset - thumb->pointOffset() , thumb->parentMargin() );
	}
	else
	{
		int valueOffset = static_cast<int>(((height() - 2.0 * grooveOffset()) * value / range) + 0.5);
		thumb->move( thumb->parentMargin() , (grooveOffset()) + valueOffset - thumb->pointOffset() );
	}
}
int MSlider::correctStickValue(MSliderThumb *thumb, int value)
{
	if(thumb == defaultThumb() || !m_stickToDefSlider)
		return value;
	int dValue = abs( defaultThumb()->value() - value );
	int delta = maximum()*m_stickDistance/1000;
	if(dValue < delta)
		return defaultThumb()->value();
	return value;
}

void MSlider::thumbToggled(bool isChecked)
{
	if (m_recursionGuard)
		return;

	m_recursionGuard = true;
	{
		MSliderThumb *senderThumb = static_cast<MSliderThumb *>(sender());

		for (MSliderThumb * thumb : m_thumbsByName)
		{
			if (thumb != senderThumb && thumb->isChecked())
				thumb->setChecked(false);
		}

		m_selectedThumb = (isChecked ? senderThumb : defaultThumb());
	}
	m_recursionGuard = false;
}

void MSlider::updateBackground()
{
	m_emptyBackground    = QPixmap( width() , height() );
	m_normalBackground   = QPixmap( width() , height() );
	m_selectedBackground = QPixmap( width() , height() );
	m_borderBackground   = QPixmap( width() , height() );

	m_emptyBackground    .fill( QColor(0,0,0,0) );
	m_normalBackground   .fill( QColor(0,0,0,0) );
	m_selectedBackground .fill( QColor(0,0,0,0) );
	m_borderBackground   .fill( QColor(0,0,0,0) );

	QString grooveStyle;
	int grooveLength = 0;

	if ( orientation() == Qt::Horizontal )
	{
		grooveStyle = " QSlider                      { background:rgba(0,0,0,0); } "
					  " QSlider::groove:horizontal   { width:%1; %2 } "
					  " QSlider::handle:horizontal   { %3 } "
					  " QSlider::add-page:horizontal { %4 } "
					  " QSlider::sub-page:horizontal { %5 } ";
		grooveLength = width() - 2*(grooveOffset());
	}
	else
	{
		grooveStyle = " QSlider                      { background:rgba(0,0,0,0); } "
					  " QSlider::groove:vertical     { height:%1; %2 } "
					  " QSlider::handle:vertical     { %3 } "
					  " QSlider::add-page:horizontal { %4 } "
					  " QSlider::sub-page:horizontal { %5 } ";
		grooveLength = height() - 2*(grooveOffset());
	}

	m_paintHelper->setParent(nullptr);
	m_paintHelper->setOrientation( orientation() );
	m_paintHelper->setMinimumSize( QSize( width() , height() ) );
	m_paintHelper->setMaximumSize( QSize( width() , height() ) );

	m_paintHelper->setStyleSheet( QString(grooveStyle).arg(grooveLength).arg( grooveEmptyStyle() ).arg(handleStyle()).arg(addPageStyle()).arg(subPageStyle()) );
	m_paintHelper->render( &m_emptyBackground );

	m_paintHelper->setStyleSheet( QString(grooveStyle).arg(grooveLength).arg( grooveNormalStyle() ).arg(handleStyle()).arg(addPageStyle()).arg(subPageStyle()) );
	m_paintHelper->render( &m_normalBackground );

	m_paintHelper->setStyleSheet( QString(grooveStyle).arg(grooveLength).arg( grooveSelectedStyle() ).arg(handleStyle()).arg(addPageStyle()).arg(subPageStyle()) );
	m_paintHelper->render( &m_selectedBackground );

	m_paintHelper->setStyleSheet( QString(grooveStyle).arg(grooveLength).arg( grooveBorderStyle() ).arg(handleStyle()).arg(addPageStyle()).arg(subPageStyle()) );
	m_paintHelper->render( &m_borderBackground );
	m_paintHelper->setParent(this);

	update();
}

void MSlider::updateThumbLayout()
{
	int minZ = 0, maxZ = 0;
	for (MSliderThumb * thumb : m_thumbsByName)
	{
		minZ = std::min( minZ, thumb->zOrder() );
		maxZ = std::max( maxZ, thumb->zOrder() );
	}

	for (int curZ = minZ; curZ <= maxZ; ++curZ)
		for (MSliderThumb * thumb : m_thumbsByName)
			if (thumb->zOrder() == curZ)
				thumb->raise();
}


void MSlider::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_isPressed)
	{
		m_isPressed = false;

		if (auto thumb = positionableThumb())
			thumb->setDown(m_isPressed);

		setSliderDown(false);
	}

	event->accept();
}

void MSlider::mousePressEvent(QMouseEvent *event)
{
	if ( event->button() == Qt::LeftButton )
	{
		//вместо генерации сигналов sliderPressed, sliderReleased нужно использовать setSliderDown,
		//т.к. тогда при возникновении valueChanged будет возникать сигнал sliderMoved (сигнализирует о том, что пользователь меняет значение)
		setSliderDown(true);

		int value = pointToValue( event->pos() );

		auto thumb = positionableThumb();
		if (thumb && thumb->isPositionable())
		{
			thumb->setValue(value);
			m_isPressed = true;
			thumb->setDown(m_isPressed);
		}
	}
	event->accept();
}

void MSlider::mouseMoveEvent(QMouseEvent *event)
{
	if ( m_isPressed )
	{
		int value = pointToValue( event->pos() );

		if (auto thumb = positionableThumb())
			thumb->setValue(value);
	}

	event->accept();
}

void MSlider::resizeEvent(QResizeEvent *event)
{
	updateThumbsValues();
}

void MSlider::sliderChange(SliderChange change)
{
	QSlider::sliderChange(change);

	if(change == SliderRangeChange)
		updateThumbsValues();
}

void MSlider::updateThumbsValues()
{
	for (const auto thumb : m_thumbsByName)
	{
		thumb->blockSignals(true);
		thumb->setValue( thumb->value() );
		thumb->blockSignals(false);
	}
}

void MSlider::paintEvent(QPaintEvent *event)
{
	bool needUpdateBackground = m_isBackgroundChanged;
	if (m_emptyBackground.width() != width() || m_emptyBackground.height() != height())
		{ needUpdateBackground = true; }
	if (minimum() != m_paintHelper->minimum() || maximum() != m_paintHelper->maximum())
		{ m_paintHelper->setRange(minimum(), maximum()); needUpdateBackground = true; }
	if (value() != m_paintHelper->value())
		{ m_paintHelper->setValue(value()); needUpdateBackground = true; }
	if (needUpdateBackground)
		{ updateBackground(); m_isBackgroundChanged = false; }

	{
		QPainter painter(this);

		painter.drawPixmap( QPoint(0,0) , m_emptyBackground );

		double offset = minimum();
		double range = maximum() - minimum();

		QList<Range>::iterator I , E;

		for( I = m_ranges.begin(), E = m_ranges.end() ; I != E ; ++I )
		{
			int from = (grooveOffset()) + (width() - 2*(grooveOffset())) * double(I->from - offset) / range;
			int to   = (grooveOffset()) + (width() - 2*(grooveOffset())) * double(I->to   - offset) / range;

			QRect rect( QPoint(from, 0), QPoint(to, height()) );

			QPixmap &pixmap = I->selected ? m_selectedBackground : m_normalBackground;

			painter.drawPixmap( rect , pixmap , rect );
		}

		for( I = m_ranges.begin(), E = m_ranges.end() ; I != E ; ++I )
		{
			int from = (grooveOffset()) + (width() - 2*(grooveOffset())) * double(I->from - offset) / range;
			int to   = (grooveOffset()) + (width() - 2*(grooveOffset())) * double(I->to   - offset) / range;

			// if ( from != (grooveOffset()+1) || to == (grooveOffset()+1) )
			{
				QRect fromBorderRect( QPoint(from, 0), QPoint(from, width()) );
				painter.drawPixmap( fromBorderRect , m_borderBackground , fromBorderRect );
			}

			QRect toBorderRect( QPoint(to, 0), QPoint(to, width()) );
			painter.drawPixmap( toBorderRect , m_borderBackground , toBorderRect );
		}
	}

	QSlider::paintEvent(event);
}

MSliderThumb * MSlider::positionableThumb() const
{
	QList<MSliderThumb *> order;
	order << selectedThumb() << defaultThumb();
	for (const auto thumb : order)
	{
		if (thumb && thumb->isPositionable())
			return thumb;
	}
	return nullptr;
}

MSliderThumb *MSlider::addThumb(QString name)
{
	removeThumb(name);

	MSliderThumb *thumb = new MSliderThumb(this);
	thumb->setFocusPolicy(Qt::ClickFocus);
	thumb->setName(name);
	thumb->setStyleTag(name); // это оставил, чтобы не ломать старый код, когда вместо name все ручки хранились под своим стилем
	m_thumbsByName[name] = thumb;
	connect( thumb , SIGNAL(toggled(bool)) , this , SLOT(thumbToggled(bool)) );
	// thumb->setParent( m_paintHelper );

	connect( thumb , &MSliderThumb::dragStarted , this , [this]() {
		setSliderDown(true);
	});

	if (name == "default")
	{
		m_defaultThumb = thumb;

		connect( thumb , SIGNAL(valueChanged(int)) , this , SLOT(defaultSliderValueChanged(int)) );
		connect( this  , SIGNAL(valueChanged(int)) , this , SLOT(defaultSliderSetValue(int)) );
	}

	connect( thumb, &MSliderThumb::dragFinished, this, [this]() {
		setSliderDown(false);
	});

	return thumb;
}

void MSlider::removeThumb(QString name)
{
	MSliderThumb *thumb = thumbByName(name);

	if (m_selectedThumb == thumb)
		m_selectedThumb = m_defaultThumb;

	if (m_defaultThumb == thumb)
	{
		if (m_selectedThumb == m_defaultThumb)
			m_selectedThumb = nullptr;
		m_defaultThumb = nullptr;
	}

	if (m_selectedThumb == thumb)
		m_selectedThumb = NULL;

	delete thumb;

	m_thumbsByName.remove(name);
}

void MSlider::defaultSliderSetValue(int value)
{
	if (m_recursionGuard)
		return;

	m_recursionGuard = true;
	{
		m_defaultThumb->setValue(value);
	}
	m_recursionGuard = false;
}

void MSlider::defaultSliderValueChanged(int value)
{
	if (m_recursionGuard)
		return;

	m_recursionGuard = true;
	{
		setValue(value);
	}
	m_recursionGuard = false;
}
