#ifndef QT_IMAGE_PROVIDER_OLD_HPP
#define QT_IMAGE_PROVIDER_OLD_HPP

#include <memory>
#include "wg_types/binary_block.hpp"

#include <QQuickImageProvider>
#include <QHash>
#include <QString>
#include <QImage>

class QtImageProviderOld : public QQuickImageProvider
{
public:
	QtImageProviderOld();

	QString encodeImage( const std::shared_ptr< BinaryBlock > & image );
	QImage requestImage( const QString &id, QSize *size, const QSize& requestedSize );

	static const char * providerId();

private:
	// TODO cap the size of this cache
	QHash<QString, QImage> imageCache_;
};

#endif // QT_IMAGE_PROVIDER_OLD_HPP