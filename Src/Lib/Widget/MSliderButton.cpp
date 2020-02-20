#include "MSliderButton.h"

#include <cassert>

#include <QVariant>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QLayout>

MSliderButton::MSliderButton( QWidget *parent )
	: QWidget( parent )
	, m_slider(nullptr)
	, m_sliderFrame(nullptr)
	, m_buttonStack(nullptr)
	, m_oldValue(0)
	, m_state(false)
	, m_onTop(false)
	, m_shadow(true)
	, m_transparentBackground(false)
	, m_sliderHeight(120)
	, m_sliderShiftX(0)
	, m_sliderShiftY(0)
{
	m_slider = new QSlider( Qt::Vertical );
	m_slider->setObjectName("slider");
	m_slider->installEventFilter(this);

	m_slider->setMinimum( 0 );
	m_slider->setMaximum( 100 );

	m_sliderFrame = new QFrame(this);
	m_sliderFrame->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
	m_sliderFrame->setObjectName("sliderFrame");
	QHBoxLayout *l = new QHBoxLayout;
	l->setMargin(0);
	l->setContentsMargins(0, 5, 0, 5);
	l->addStretch(1);
	l->addWidget(m_slider);
	l->addStretch(1);
	m_sliderFrame->setLayout(l);
	m_sliderFrame->hide();
}

MSliderButton::~MSliderButton()
{
	if (m_sliderFrame && !m_sliderFrame->parent())
		delete m_sliderFrame;
}

void MSliderButton::setVolumeLevelsNumber(int const volumelevelsNumber)
{
	m_buttonStack = new QStackedWidget(this);
	m_buttonStack->setProperty( "styleTag" , "MSliderButtonMain" );

	for (int i = 0; i < volumelevelsNumber; ++i)
	{
		auto button = new QToolButton;
		button->setCheckable(true);
		button->setProperty("styleTag" , QString("MSliderButton%1").arg(i));
		button->setFocusPolicy(Qt::NoFocus);

		connect(button , &QToolButton::clicked , this , &MSliderButton::showSlider);

		// Если при открытом слайдере нажать за его пределами (чтобы скрыть),
		// то у самой кнопки остаётся состояние hovered с соответствующим цветом
		// подробнее https://bugreports.qt.io/browse/QTBUG-36862
		connect(button , &QToolButton::toggled , [button](bool checked)
		{
			if (!checked)
				button->setAttribute(Qt::WA_UnderMouse, button->rect().contains(button->mapFromGlobal(QCursor::pos())));
		});

		m_buttonStack->addWidget(button);
	}

	m_buttonStack->setCurrentIndex(m_buttonStack->count() - 1);

	connect(m_slider , &QSlider::valueChanged, this , &MSliderButton::setValue);
}

void MSliderButton::setButtonState(bool state)
{
	for (int i = 0; i < m_buttonStack->count(); ++i)
	{
		if (auto button = qobject_cast<QToolButton *>(m_buttonStack->widget(i)))
			button->setChecked(state);
	}
}

void MSliderButton::setValue( int value )
{
	assert(m_buttonStack);
	if (!m_buttonStack)
		return;

	auto const min = m_slider->minimum();
	auto const max = m_slider->maximum();

	value = qBound(min, value, max);

	m_slider->blockSignals(true);
	m_slider->setValue(value);
	m_slider->blockSignals(false);

	auto difference = max - min;

	auto maxButtonIndex = m_buttonStack->count() - 1;
	m_buttonStack->setCurrentIndex(value == min ? 0 : qMin((value - min) * (maxButtonIndex) / difference + 1, maxButtonIndex ));

	emit valueChanged(value);
}

void MSliderButton::showSlider()
{
	m_sliderFrame->resize( width() , m_sliderHeight );

	QPoint pos = mapToGlobal(QPoint(0,0));
	m_sliderFrame->move(pos.x() + m_sliderShiftX, pos.y() - m_sliderHeight + m_sliderShiftY);

	m_sliderFrame->show();
	m_slider->setFocus();
}

void MSliderButton::toggle()
{
	toggle(!m_state);
}

void MSliderButton::toggle(bool state)
{
	if (!state)
	{
		m_oldValue = value();
		setValue(0);
	}
	else
	{
		setValue(m_oldValue);
	}
	m_state = state;

	emit toggled(m_state);
}

bool MSliderButton::eventFilter(QObject *obj, QEvent *event)
{
	QSlider *slider = qobject_cast<QSlider *>(obj);
	if (slider != m_slider)
		return false;

	switch (event->type())
	{
	case QEvent::FocusIn:
		setButtonState(true);
		break;
	case QEvent::FocusOut:
		setButtonState(false);
		m_sliderFrame->hide();
		break;
	default:
		break;
	}

	return false;
}

void MSliderButton::setOnTop(bool top)
{
	if (top == m_onTop)
		return;
	m_onTop = top;
	auto flags = m_sliderFrame->windowFlags();
	m_sliderFrame->setWindowFlags(top ? ((flags ^ Qt::Popup) | Qt::SplashScreen) : ((flags ^ Qt::SplashScreen) | Qt::Popup));
	m_sliderFrame->setParent(top ? nullptr : this);
}

void MSliderButton::setTransparentBackground(bool transparentBackground)
{
	if (transparentBackground)
		m_sliderFrame->setAttribute(Qt::WA_TranslucentBackground);
}

void MSliderButton::setSliderHeight(int height)
{
	m_sliderHeight = height;
}

void MSliderButton::setSliderShiftX(int ShiftX)
{
	m_sliderShiftX = ShiftX;
}

void MSliderButton::setSliderShiftY(int ShiftY)
{
	m_sliderShiftY = ShiftY;
}
