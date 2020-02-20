#include <QWidget>
#include <QStackedWidget>
#include <QToolButton>
#include <QSlider>
#include <QStackedWidget>

#include "MovaviWidgetLib.h"

/**
 * @brief Контрол регулировки значения (например, уровня звука)
 * Виджет состоит из стэка QToolButton и popup-фрейма со слайдером
 */

class MOVAVIWIDGET_API MSliderButton : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(bool onTop                 READ onTop                 WRITE setOnTop                 DESIGNABLE true)
	Q_PROPERTY(bool shadow                READ shadow                WRITE setShadow                DESIGNABLE true)
	Q_PROPERTY(bool transparentBackground READ transparentBackground WRITE setTransparentBackground DESIGNABLE true)
	Q_PROPERTY(int  sliderHeight          READ sliderHeight          WRITE setSliderHeight          DESIGNABLE true)
	// смещение popup-фрейма (m_sliderFrame) относительно QToolButton
	Q_PROPERTY(int  sliderShiftX          READ sliderShiftX          WRITE setSliderShiftX          DESIGNABLE true)
	Q_PROPERTY(int  sliderShiftY          READ sliderShiftY          WRITE setSliderShiftY          DESIGNABLE true)

	QSlider			*m_slider;
	QFrame			*m_sliderFrame;

	QStackedWidget	*m_buttonStack;

	int				m_oldValue;
	bool			m_state;
	bool			m_onTop;
	bool			m_shadow;
	bool            m_transparentBackground;
	int             m_sliderHeight;
	int             m_sliderShiftX;
	int             m_sliderShiftY;

	bool eventFilter( QObject * , QEvent * );

	void setButtonState(bool);

public:
	MSliderButton( QWidget *parent = NULL );
	~MSliderButton();

	void setVolumeLevelsNumber(int const volumelevelsNumber);

	int value() const { return m_slider->value(); }
	int minimum() const { return m_slider->minimum(); }
	int maximum() const { return m_slider->maximum(); }

	bool onTop() const { return m_onTop; }
	void setOnTop(bool);

	bool shadow() const { return m_shadow; }
	void setShadow(bool);

	bool transparentBackground() const { return m_transparentBackground; }
	void setTransparentBackground(bool);

	bool sliderHeight() const { return m_sliderHeight; }
	void setSliderHeight(int);

	bool sliderShiftX() const { return m_sliderShiftX; }
	void setSliderShiftX(int);

	bool sliderShiftY() const { return m_sliderShiftY; }
	void setSliderShiftY(int);

signals:
	void valueChanged(int);
	void toggled(bool);

public slots:
	void setValue(int);
	void toggle();
	void toggle(bool);

protected slots:
	void showSlider();
};
