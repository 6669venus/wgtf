#include "qt_image_provider.hpp"

QtImageProvider::QtImageProvider()
	: QQuickImageProvider( ImageType::Image )
{
}

QString QtImageProvider::encodeImage( const std::shared_ptr< BinaryBlock > & image )
{
	char buffer[ 65535 ];
	sprintf_s( buffer, sizeof(buffer), "%p", image->data() );
	QString imagePath( buffer );
	auto it = imageCache_.find( imagePath );
	if (it == imageCache_.end())
	{
		QByteArray data;
		data.setRawData( image->cdata(), ( uint )( image->length() ) );
		imageCache_[ imagePath ] = QImage::fromData( data, "BMP" );
	}
	return "image://" + QString( providerId() ) + "/" + imagePath;
}

QImage QtImageProvider::requestImage( 
	const QString &id, QSize *size, const QSize& requestedSize )
{
	auto it = imageCache_.find( id );
	if (it != imageCache_.end())
	{
		return it.value();
	}

	return QImage( 
		requestedSize.width(), requestedSize.height(), QImage::Format_ARGB32 );
}

const char * QtImageProvider::providerId()
{
	return "QtImageProvider";
}