#pragma once

#include <QWidget>
#include <QSlider>
#include <QMap>
#include <QToolButton>

#include "MovaviWidgetLib.h"

#define WHEEL_STEP 120

class MSlider;
class MSliderThumb;
class MMagnet;

/** @class MSliderThumb
 *  @brief Ручка для слайдера MSlider
 *
 *  Это фактически QToolButton, который можно передвигать по области слайдера. У него есть
 *  нескольго дополнительных свойств, сигналов и стилей, которые описаны ниже. ВАЖНО: вместо
 *  isCheckable/setCheckable нужно использовать isSelectable/setIsSelectable, поведение у них
 *  то же самое, но они будут работать правильно, а оригинальные функции QToolButton - нет.
 *  Только одна ручка на слайдере может быть checked (при выставлении ей checked остальные
 *  станут unchecked автоматически).
 */
class MOVAVIWIDGET_API MSliderThumb : public QToolButton // only QToolButton has required styles and properties
{
	Q_OBJECT

	Q_PROPERTY( QString name         READ name         WRITE setName         )
	Q_PROPERTY( QString styleTag     READ styleTag     WRITE setStyleTag     )
	Q_PROPERTY( bool    isSelectable READ isSelectable WRITE setIsSelectable )
	Q_PROPERTY( bool    isDraggable  READ isDraggable  WRITE setIsDraggable  )
	Q_PROPERTY( int     pointOffset  READ pointOffset  WRITE setPointOffset  )
	Q_PROPERTY( int     parentMargin READ parentMargin WRITE setParentMargin )
	Q_PROPERTY( int     zOrder       READ zOrder       WRITE setZOrder       )

public:
	MSliderThumb(MSlider *parent);

	~MSliderThumb();

	QString name()           const { return m_name; }           ///< Имя, под которым ручка хранится в слайдере
	QString styleTag()       const { return m_styleTag; }       ///< Стиль ручки
	bool    isSelectable()   const { return m_isSelectable; }   ///< Можно ли выбрать ручку кликом мышью (это вызовет состояние checked у QToolButton)
	bool    isDraggable()    const { return m_isDraggable; }    ///< Moжно ли перетаскивать ручку по слайдеру мышью
	bool    isPositionable() const { return m_isPositionable; } ///< Ручку можно спозиционировать по клику в области слайдера
	int     pointOffset()    const { return m_pointOffset; }    ///< Отступ в пикселях от края ручки до места, указывающего значение на слайдере
	int     parentMargin()   const { return m_parentMargin; }   ///< Отступ в пикселях от края слайдера до края ручки (перпендикулярно "щели")
	int     zOrder()         const { return m_zOrder; }         ///< Задает порядок отрисовки (ручки с большим zOrder рисуются поверх ручек с меньшим)

	bool isMovableByMouseWheel() const { return m_isMovableByMouseWheel; } ///< Можно ли сдвинуть с помощью колеса мыши
	bool isMovableByKeyboard()   const { return m_isMovableByKeyboard; }   ///< Можно ли сдвинуть с помощью кнопок клавиатуры

	void setName           (QString name){ m_name           = name; }
	void setStyleTag       (QString tag) { m_styleTag       = tag; repolish( ); }
	void setIsSelectable   (bool flag)   { m_isSelectable   = flag; }
	void setIsDraggable    (bool flag)   { m_isDraggable    = flag; }
	void setIsPositionable (bool flag)   { m_isPositionable = flag; }
	void setPointOffset    (int offset)  { m_pointOffset    = offset; setValue(m_value); }
	void setParentMargin   (int margin)  { m_parentMargin   = margin; setValue(m_value); }
	void setZOrder         (int order)   { m_zOrder         = order; relayout(); }

	void setIsMovableByMouseWheel(bool flag) { m_isMovableByMouseWheel = flag; }
	void setIsMovableByKeyboard(bool flag) { m_isMovableByKeyboard = flag; }

	int  value()   { return m_value; }   ///< значение, на которое указывает ручка (в терминах слайдера)
	int  minimum() { return m_minimum; } ///< минимально возможное значение для данной ручки
	int  maximum() { return m_maximum; } ///< максимально возможное значение для данной ручки

	void setValue(int value);
	void setMinimum(int value);
	void setMaximum(int value);

	/// @brief Возвращает 'магнит', для управления 'примагничеваением'
	/// @note В случаи необходимости инициализирует m_magnet
	MMagnet& magnet();

signals:
	void valueChanged(int); ///< срабатывает при изменении значения ручки (любым способом)
	void dragStarted();     ///< срабатывает ПОСЛЕ начала перетаскивания ручки по слайдеру
	void dragFinished();    ///< срабатывает при отпускании перетаскиваемой кнопки
	void stepped(int factor);///< срабатывает при "шаге", полученном от ролика мыши либо от курсорных клавиш. factor - величина шага.

protected:
	virtual void moveEvent         (QMoveEvent  *) override;
	virtual void mousePressEvent   (QMouseEvent *) override;
	virtual void mouseReleaseEvent (QMouseEvent *) override;
	virtual void mouseMoveEvent    (QMouseEvent *) override;
	virtual void wheelEvent        (QWheelEvent *) override;
	virtual void keyPressEvent     (QKeyEvent   *) override;
	virtual QSize minimalSizeHint() const { return QSize(1,1); }

private:
	friend class MSlider;

	MSlider *m_parent;

	/// @brief Магнит, для примагничевая к чему-либо
	/// @details Создаётся только в случаи, когда пользователь запросил  magnet()
	MMagnet *m_magnet;

	QPoint m_pressedAt;

	QString m_name;
	QString m_styleTag;
	bool m_isPressed;
	bool m_isMoved;

	int m_value;
	int m_maximum;
	int m_minimum;

	int m_zOrder;       // both painting and interaction is affected
	int m_pointOffset;  // in px, from button left/top to the groove point
	int m_parentMargin; // in px, from parent left/top to button left/top

	bool m_isSelectable;
	bool m_isDraggable;
	bool m_isPositionable;
	bool m_isMovableByMouseWheel;
	bool m_isMovableByKeyboard;

	void repolish();
	void relayout();

};

/** @class MSlider
 *  @brief Слайдер с произвольным количеством ручек и отрисовкой набора интервалов на "щели"
 *
 * Интерфейс повторяет обычный QSlider с несколькими отличиями. Самое главное - обычной ручки
 * нет, вместо нее есть набор ручек - объектов MSliderThumb, идентифицируемых тэгами.
 * Ручка с тэгом default создается по умолчанию и подключена к стандартным слотам/сигналам
 * слайдера (т.е. `QSlider::setValue(int)` установит ее значение, а при перетаскивании ее
 * мышкой сработает `QSlider::valueChanged(int)`). Дополнительные ручки нужно создавать в
 * коде, и работать с ними как с отдельными объектами типа MSliderThumb.
 *
 * Интервалы задаются как набор структур, содержащих начало и конец интервала в единицах слайдера,
 * и статус "выделенности". Они не должны пересекаться друг с другом. Отрисовка выполняется путем
 * составления фона слайдера "по кусочкам" из четырех по разному стилизованных фонов слайдера (это
 * позволяет поддерживать сложные варианты стилей - закругленные границы, градиенты и т.д.). Сначала
 * слайдер заполняется фоном со стилем grooveEmpty, затем поверх него рисуются кусочки для каждого
 * интервала, вырезанные пропорционально его началу/концу. При этом используется стиль grooveNormal
 * для невыделенного интервала и grooveSelected для выделенного. После этого на границах каждого
 * интервала рисуются кусочки шириной в один пиксель со стилем grooveBorder (это позволяет различать
 * границы у вплотную примыкающих интервалов и видеть интервалы шириной менее пикселя). Эти стили
 * задаются как property-строки, которые вставляются внутрь описания стиля элемента QSlider::groove.
 */
class MOVAVIWIDGET_API MSlider : public QSlider
{
	Q_OBJECT

	Q_PROPERTY( int     grooveOffset        READ grooveOffset        WRITE setGrooveOffset        )
	Q_PROPERTY( QString grooveEmptyStyle    READ grooveEmptyStyle    WRITE setGrooveEmptyStyle    )
	Q_PROPERTY( QString grooveNormalStyle   READ grooveNormalStyle   WRITE setGrooveNormalStyle   )
	Q_PROPERTY( QString grooveSelectedStyle READ grooveSelectedStyle WRITE setGrooveSelectedStyle )
	Q_PROPERTY( QString grooveBorderStyle   READ grooveBorderStyle   WRITE setGrooveBorderStyle   )
	Q_PROPERTY( QString addPageStyle        READ addPageStyle        WRITE setAddPageStyle        )
	Q_PROPERTY( QString subPageStyle        READ subPageStyle        WRITE setSubPageStyle        )
	Q_PROPERTY( QString handleStyle         READ handleStyle         WRITE setHandleStyle         )

public:
	int     grooveOffset        () { return m_grooveOffset;        } ///< отступ "щели" от краев виджета в пикселях (параллельно движению ручек)
	QString grooveEmptyStyle    () { return m_grooveEmptyStyle;    } ///< попадает в стиль grooveEmpty
	QString grooveNormalStyle   () { return m_grooveNormalStyle;   } ///< попадает в стиль grooveNormal
	QString grooveSelectedStyle () { return m_grooveSelectedStyle; } ///< попадает в стиль grooveSelected
	QString grooveBorderStyle   () { return m_grooveBorderStyle;   } ///< попадает в стиль grooveBorder
	QString addPageStyle        () { return m_addPageStyle;        } ///< попадает в стиль для стилизации add-page
	QString subPageStyle        () { return m_subPageStyle;        } ///< попадает в стиль для стилизации sub-page
	QString handleStyle         () { return m_handleStyle;         } ///< попадает в стиль для стилизации handle

	void setGrooveOffset        (int     x) { m_grooveOffset        = x; m_isBackgroundChanged = true; updateThumbLayout(); }
	void setGrooveEmptyStyle    (QString x) { m_grooveEmptyStyle    = x; m_isBackgroundChanged = true; }
	void setGrooveNormalStyle   (QString x) { m_grooveNormalStyle   = x; m_isBackgroundChanged = true; }
	void setGrooveSelectedStyle (QString x) { m_grooveSelectedStyle = x; m_isBackgroundChanged = true; }
	void setGrooveBorderStyle   (QString x) { m_grooveBorderStyle   = x; m_isBackgroundChanged = true; }
	void setAddPageStyle        (QString x) { m_addPageStyle        = x; m_isBackgroundChanged = true; } // not sure about m_isBackgroundChanged, but let it be
	void setSubPageStyle        (QString x) { m_subPageStyle        = x; m_isBackgroundChanged = true; }
	void setHandleStyle         (QString x) { m_handleStyle         = x; m_isBackgroundChanged = true; }

	void setStickDistance(int dst) { m_stickDistance = dst; }	///< установка дистанции прилипания маркеров, измеряется в 1/1000 от длины ролика, так что ставить больше 1000 безвредно, но смысла не имеет.
	void setStickEnable(bool enable) { m_stickToDefSlider = enable; } ///< установка прилипания маркеров к дефолтному маркеру. По-умолчанию выключена.

	struct Range ///< структура, описывающая интервал на "щели"
	{
		int from, to;
		bool selected;

		Range(int F = 0, int T = 0, bool S = false)
			: from(F), to(T), selected(S) { }
	};
	QList<Range> const & ranges()     { return m_ranges; } ///< набор интервалов, отрисованный на слайдере
	void setRanges(QList<Range> list) { m_ranges = list; m_isBackgroundChanged = true; }


	MSliderThumb *selectedThumb() const { return m_selectedThumb; } ///< выбранная ручка (может быть только одна на слайдер)
	MSliderThumb *defaultThumb()  const { return m_defaultThumb; }  ///< ручка по умолчанию (реагирует на мышь, когда нет выбранной)
	MSliderThumb *positionableThumb() const; ///< Ручка, которую можно позиционировать кликом левой кнопкий мыши (приоритет: selectedThumb, defaultThumb)

	MSliderThumb *addThumb   (QString name); ///< добавляет ручку с именем name
	MSliderThumb *thumbByName(QString name) { return m_thumbsByName.value(name); } ///< возвращает существующую ручку с именем name
	void          removeThumb(QString name); ///< удаляет ручку с именем name

	MSlider(QWidget *parent = 0);

	/// Эти функции использует MSliderThumb, вызывать их напрямую не следует
	/// @{
	void updateBackground();

	void updateThumbLayout();

	void moveThumbTo(MSliderThumb *thumb, int value);

	int correctStickValue(MSliderThumb *thumb, int value); ///< корректирует значение перетаскиваемого слайдера thumb в зависимости от значения defaultThumb

	int pointToValue(QPoint point);
	/// @}

private slots:
	void thumbToggled(bool flag);

	void defaultSliderSetValue(int value);
	void defaultSliderValueChanged(int value);

protected:
	virtual void resizeEvent       (QResizeEvent *) override;
	virtual void mousePressEvent   (QMouseEvent *) override;
	virtual void mouseReleaseEvent (QMouseEvent *) override;
	virtual void mouseMoveEvent    (QMouseEvent *) override;
	virtual void paintEvent        (QPaintEvent *) override;
	virtual void wheelEvent        (QWheelEvent *) override;
	virtual void keyPressEvent     (QKeyEvent   *) override;
	virtual void changeEvent       (QEvent *) override;

	void sliderChange(SliderChange change) override;

private:
	void updateThumbsValues();

private:
	QSlider *m_paintHelper;

	MSliderThumb *m_selectedThumb;
	MSliderThumb *m_defaultThumb;

	QList<Range> m_ranges;
	QMap<QString, MSliderThumb *> m_thumbsByName;

	bool m_isPressed;
	bool m_recursionGuard;
	bool m_isBackgroundChanged;

	QPixmap m_emptyBackground;
	QPixmap m_normalBackground;
	QPixmap m_selectedBackground;
	QPixmap m_borderBackground;

	QString m_grooveEmptyStyle;
	QString m_grooveNormalStyle;
	QString m_grooveSelectedStyle;
	QString m_grooveBorderStyle;
	QString m_addPageStyle;
	QString m_subPageStyle;
	QString m_handleStyle;

	int m_grooveOffset;

	bool m_stickToDefSlider; ///< флаг, включающий "прилипание" дополнительных маркеров к дефолтному.
	int m_stickDistance; ///< расстояние, на котором "притягиваются" маркеры. Измеряется в тысячных долях от общей продолжительности ролика
};
