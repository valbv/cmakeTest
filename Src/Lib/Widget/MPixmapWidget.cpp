#include "MPixmapWidget.h"

#include <cassert>

#include <QMovie>
#include <QPixmap>
#include <QPointer>
#include <QPainter>
#include <QPaintEvent>
#include <QBackingStore>

namespace {

const QPainter::RenderHints RENDER_HINTS = QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform;

} // namespace

struct MPixmapWidget::Impl : public QObject
{
	Impl(MPixmapWidget* pixmapWidget)
		: pixmapWidget(pixmapWidget)
	{ }

	void initMovie(QMovie * movie)
	{
		if (this->movie)
			disconnect(this->movie.data(), &QMovie::frameChanged, this, &Impl::update);
		this->movie = movie;
		if (this->movie)
			connect(this->movie.data(), &QMovie::frameChanged, this, &Impl::update);

		update();
	}

	void initMovie(const QString & path)
	{
		ownedMoviePath = path;
		// Try to keep the same frame
		if (!ownedMovie)
			ownedMovie = new QMovie(pixmapWidget);
		int frameNumber = ownedMovie->currentFrameNumber();
		ownedMovie->stop();
		ownedMovie->setFileName(path);
		initMovie(ownedMovie);
		ownedMovie->start();
		if (frameNumber != -1 && ownedMovie->frameCount() > 0)
		{
			frameNumber = (frameNumber + 1) % ownedMovie->frameCount();
			/// XXX: QMovie::jumpToFrame не работает в некоторых ситуациях, например,
			/// если movie только что загрузил файл и ещё не проиграл весь цикл фреймов
			while (ownedMovie->currentFrameNumber() != frameNumber)
				ownedMovie->jumpToNextFrame();
		}
	}

	void update()
	{
		preparePixmap();
		pixmapWidget->update();
	}

	void preparePixmap()
	{
		scaledPixmap = QPixmap();

		QPixmap pixmap = movie
			? movie->currentPixmap()
			: originalPixmap;

		pixmap.setDevicePixelRatio(1);

		if(pixmap.isNull() || maskMode == MASK_NONE)
		{
			preparedPixmap = pixmap;
			return;
		}

		const QSize preparedSize = [this, &pixmap]{
			const qreal pixmapWidth = static_cast<qreal>(pixmap.width());
			const qreal pixmapHeight = static_cast<qreal>(pixmap.height());
			const qreal aspectRatio = static_cast<qreal>(pixmapWidget->width()) / static_cast<qreal>(pixmapWidget->height());
			if(pixmapWidth / aspectRatio > pixmapHeight)
				return QSizeF(pixmapHeight * aspectRatio, pixmapHeight).toSize();
			else
				return QSizeF(pixmapWidth, pixmapWidth / aspectRatio).toSize();
		}();

		const QRect preparedRect = [this, &preparedSize, &pixmap]{
			const QRect originalPixmapRect = pixmap.rect();
			const QRect preparedPixmapRect = QRect(QPoint(0, 0), preparedSize);
			QPoint topLeft;
			switch(maskAnchor)
			{
			case ANCHOR_CENTER:
				topLeft = originalPixmapRect.center() - preparedPixmapRect.center();
				break;
			case ANCHOR_TOP_LEFT:
				topLeft = originalPixmapRect.topLeft();
				break;
			case ANCHOR_TOP_RIGHT:
				topLeft = originalPixmapRect.topRight() - preparedPixmapRect.topRight();
				break;
			case ANCHOR_BOTTOM_LEFT:
				topLeft = originalPixmapRect.bottomLeft() - preparedPixmapRect.bottomLeft();
				break;
			case ANCHOR_BOTTOM_RIGHT:
				topLeft = originalPixmapRect.bottomRight() - preparedPixmapRect.bottomRight();
				break;
			}
			return QRect(topLeft, preparedSize);
		}();

		QPixmap rectanglePixmap(preparedSize);
		rectanglePixmap.setDevicePixelRatio(1);
		rectanglePixmap.fill(Qt::transparent);
		QPainter rectanglePainter(&rectanglePixmap);
		rectanglePainter.setRenderHints(rectanglePainter.renderHints() | RENDER_HINTS);
		rectanglePainter.drawPixmap(rectanglePixmap.rect(), pixmap, preparedRect);
		rectanglePainter.end();

		if(maskMode == MASK_RECTANGLE)
		{
			preparedPixmap = rectanglePixmap;
			return;
		}

		QPixmap ellipsePixmap(preparedSize);
		ellipsePixmap.setDevicePixelRatio(1);
		ellipsePixmap.fill(Qt::transparent);
		QPainter ellipsePainter(&ellipsePixmap);
		ellipsePainter.setRenderHints(ellipsePainter.renderHints() | RENDER_HINTS);
		ellipsePainter.setBrush(QBrush(rectanglePixmap));
		ellipsePainter.setPen(Qt::NoPen);
		ellipsePainter.drawEllipse(rectanglePixmap.rect());
		ellipsePainter.end();

		assert(maskMode == MASK_ELLIPSE);
		preparedPixmap = ellipsePixmap;
	}

	MPixmapWidget* pixmapWidget;
	MPixmapWidget::MaskMode maskMode = MASK_NONE;
	MPixmapWidget::MaskAnchor maskAnchor = ANCHOR_TOP_LEFT;
	QPixmap originalPixmap;
	QPixmap preparedPixmap;
	QPixmap scaledPixmap;

	QPointer<QMovie> movie;
	QString          ownedMoviePath;
	QMovie         * ownedMovie = nullptr;
	int              borderRadius = 0;
};

MPixmapWidget::MPixmapWidget(QWidget* parent, Qt::WindowFlags windowFlags)
	: QFrame(parent, windowFlags)
	, m_impl(new Impl(this))
{}

MPixmapWidget::~MPixmapWidget() = default;

MPixmapWidget::MaskMode MPixmapWidget::maskMode() const
{
	return m_impl->maskMode;
}

void MPixmapWidget::setMaskMode(MPixmapWidget::MaskMode mode)
{
	m_impl->maskMode = mode;
	m_impl->preparePixmap();
	update();
}

MPixmapWidget::MaskAnchor MPixmapWidget::maskAnchor() const
{
	return m_impl->maskAnchor;
}

void MPixmapWidget::setMaskAnchor(MPixmapWidget::MaskAnchor anchor)
{
	m_impl->maskAnchor = anchor;
	m_impl->preparePixmap();
	update();
}

QPixmap MPixmapWidget::pixmap() const
{
	return m_impl->originalPixmap;
}

void MPixmapWidget::setPixmap(const QPixmap& pixmap)
{
	m_impl->originalPixmap = pixmap;
	m_impl->preparePixmap();
	update();
}

QString MPixmapWidget::moviePath() const
{
	return m_impl->ownedMoviePath;
}

void MPixmapWidget::setMoviePath(const QString & path)
{
	m_impl->initMovie(path);
}

QMovie * MPixmapWidget::movie() const
{
	return m_impl->movie;
}

void MPixmapWidget::setMovie(QMovie * movie)
{
	m_impl->initMovie(movie);
}

int MPixmapWidget::borderRadius() const
{
	return m_impl->borderRadius;
}

void MPixmapWidget::setBorderRadius(int radius)
{
	m_impl->borderRadius = radius;
}

void MPixmapWidget::paintEvent(QPaintEvent* event)
{
	QFrame::paintEvent(event);
	if(m_impl->preparedPixmap.isNull())
		return;

	/// @note Здесь не получится тупо взять и нарисовать preparedPixmap, так как нам нужно его
	/// сперва отмасштабировать с Qt::SmoothTransformation, ибо, судя по качеству, по дефолту
	/// масштабирование при отрисовке использует явно не его.

	const QSize actualSize = (QSizeF(size()) * devicePixelRatioF()).toSize();
	if(m_impl->scaledPixmap.isNull() || actualSize != m_impl->scaledPixmap.size())
	{
		m_impl->scaledPixmap = QPixmap::fromImage(m_impl->preparedPixmap.toImage().scaled(actualSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		m_impl->scaledPixmap.setDevicePixelRatio(1);
	}

	QPainter painter(this);
	painter.setRenderHints(painter.renderHints() | RENDER_HINTS);
	if (auto radius = borderRadius())
	{
		QPainterPath clipPath;
		clipPath.addRoundedRect(rect(), radius, radius);
		painter.setClipPath(clipPath);
	}
	painter.drawPixmap(rect(), m_impl->scaledPixmap, m_impl->scaledPixmap.rect());
}

void MPixmapWidget::resizeEvent(QResizeEvent* event)
{
	QFrame::resizeEvent(event);
	m_impl->preparePixmap();
	update();
}
