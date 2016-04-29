#include "qt_image_provider.hpp"

#include <QIcon>

QtImageProvider::QtImageProvider()
	: QQuickImageProvider( ImageType::Image )
{
}

QString QtImageProvider::encode( const QColor &color )
{
	QPixmap pixmap(1, 1);
	pixmap.fill( color );
	return encode( pixmap );
}

QString QtImageProvider::encode( const QIcon &icon )
{
	auto availableSizes = icon.availableSizes();
	auto pixmap = icon.pixmap( availableSizes[0] );
	return encode( pixmap );
}

QString QtImageProvider::encode( const QPixmap &pixmap )
{
	auto key = pixmap.cacheKey();
	auto it = cache_.find( key );
	if (it == cache_.end())
	{
		cache_[key] = pixmap;
	}
	return "image://" + QString( providerId() ) + "/" + key;
}

QImage QtImageProvider::requestImage(const QString &id, QSize *size, const QSize& requestedSize)
{
	if (size != nullptr)
	{
		*size = requestedSize;
	}

	auto key = id.toLongLong();
	auto it = cache_.find( key );
	if (it == cache_.end())
	{
		return QImage( requestedSize.width(), requestedSize.height(), QImage::Format_ARGB32 );
	}

	auto pixmap = it.value().scaled( requestedSize );
	return pixmap.toImage();
}

const char * QtImageProvider::providerId()
{
	return "QtImageProvider";
}