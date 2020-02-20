#pragma once

#include <QFrame>
#include <QScopedPointer>

#include "MovaviWidgetLib.h"

/// @brief Виджет, умеющий рисовать QPixmap. Отличия от QLabel:
/// * QPixmap вписывается в текущие размеры виджета, логика при этом как с border-image из стилей;
/// * QPixmap рисуется с учетом DPI экрана, то есть на Retina дисплеях будем иметь четкое изображение;
/// * Поддерживается выбор масок, позволяющих получить специфически обрезанное изображение.
/// * Поддерживается работа с анимированными GIF, для этого нужно указать либо путь до файла (moviePath),
///   либо передать готовый объект QMovie. В случае установки и pixmap, и moviePath, приоритет остаётся за гифкой.
class MOVAVIWIDGET_API MPixmapWidget : public QFrame
{
	Q_OBJECT

	Q_PROPERTY(MaskMode   maskMode       READ maskMode     WRITE setMaskMode    )
	Q_PROPERTY(MaskAnchor maskAnchor     READ maskAnchor   WRITE setMaskAnchor  )
	Q_PROPERTY(QPixmap    pixmap         READ pixmap       WRITE setPixmap      )
	Q_PROPERTY(QString    moviePath      READ moviePath    WRITE setMoviePath   )
	Q_PROPERTY(int        borderRadius   READ borderRadius WRITE setBorderRadius)

public:
	MPixmapWidget(QWidget* parent = nullptr, Qt::WindowFlags windowFlags = {});
	~MPixmapWidget();

	enum MaskMode
	{
		MASK_NONE,      ///< Не использовать маску (pixmap масштабируется до размера виджета без учета пропорций)
		MASK_RECTANGLE, ///< Использовать прямоугольную маску (pixmap масштабируется до размера виджета с учетом пропорций, излишки обрезаются)
		MASK_ELLIPSE,   ///< Использовать маску-эллипс (то же, что и MASK_RECTANGLE, только дополнительно вырезается эллипс по размеру виджета)
	};
	Q_ENUM(MaskMode)

	MaskMode maskMode() const;
	void setMaskMode(MaskMode mode);

	enum MaskAnchor
	{
		ANCHOR_CENTER,
		ANCHOR_TOP_LEFT,
		ANCHOR_TOP_RIGHT,
		ANCHOR_BOTTOM_LEFT,
		ANCHOR_BOTTOM_RIGHT,
	};
	Q_ENUM(MaskAnchor)

	MaskAnchor maskAnchor() const;
	void setMaskAnchor(MaskAnchor anchor);

	QPixmap pixmap() const;
	void setPixmap(const QPixmap & pixmap);

	QString moviePath() const;
	void setMoviePath(const QString & path);

	/// NOTE: управление за жизнью объекта на совести вызывающего кода.
	/// MPixmapWidget не берёт на себя управления.
	QMovie * movie() const;
	void setMovie(QMovie *);

	int borderRadius() const;
	void setBorderRadius(int radius);

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	struct Impl;
	QScopedPointer<Impl> m_impl;
};
