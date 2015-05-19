#include "thumbnail_extension.hpp"
#include "data_model/i_item.hpp"
#include "data_model/i_list_model.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "qt_common/models/qt_list_model.hpp"
#include "qt_common/qt_image_provider.hpp"

ThumbnailExtension::ThumbnailExtension()
{
	qtFramework_ = Context::queryInterface< IQtFramework >();
}

ThumbnailExtension::~ThumbnailExtension()
{
}

QHash< int, QByteArray > ThumbnailExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( ThumbnailRole::role_, roleNames );
	return roleNames;
}

QVariant ThumbnailExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	assert( index.isValid() );
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ThumbnailRole::roleId_ )
	{
		auto thumbnail = item->getThumbnail( column );
		if (thumbnail != nullptr)
		{
			auto qtImageProvider = dynamic_cast< QtImageProvider * >(
				qtFramework_->qmlEngine()->imageProvider( QtImageProvider::providerId() ) );
			if (qtImageProvider != nullptr)
			{
				return qtImageProvider->encodeImage( thumbnail );
			}
		}
	}

	return QVariant( QVariant::Invalid );
}

bool ThumbnailExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	return false;
}