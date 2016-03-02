#include "asset_item_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/asset_browser/i_asset_object_item.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_image_provider.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"

AssetItemExtension::AssetItemExtension()
{	
	qtFramework_ = Context::queryInterface< IQtFramework >();
}

AssetItemExtension::~AssetItemExtension()
{
}

QHash< int, QByteArray > AssetItemExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( StatusIconRole::roleName_, roleNames );
	registerRole( TypeIconRole::roleName_, roleNames );
	registerRole( SizeRole::roleName_, roleNames );
	registerRole( CreatedTimeRole::roleName_, roleNames );
	registerRole( ModifiedTimeRole::roleName_, roleNames );
	registerRole( AccessedTimeRole::roleName_, roleNames );
	registerRole( IsDirectoryRole::roleName_, roleNames );
	registerRole( IsReadOnlyRole::roleName_, roleNames );
	registerRole( IsCompressedRole::roleName_, roleNames );

	return roleNames;
}

QVariant AssetItemExtension::data( const QModelIndex &index, int role ) const
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

	if (roleId == TypeIconRole::roleId_ ||
		roleId == SizeRole::roleId_ ||
		roleId == CreatedTimeRole::roleId_ ||
		roleId == ModifiedTimeRole::roleId_ ||
		roleId == AccessedTimeRole::roleId_ ||
		roleId == IsDirectoryRole::roleId_ ||
		roleId == IsReadOnlyRole::roleId_ ||
		roleId == IsCompressedRole::roleId_)
	{
		return QtHelpers::toQVariant( item->getData( column, roleId ) );
	}
	else if (roleId == StatusIconRole::roleId_)
	{		
		// Don't dynamically cast unless the status role is explicitly requested
		auto assetObjectItem = dynamic_cast< IAssetObjectItem * >( item );
		assert( assetObjectItem != nullptr );

		auto status = assetObjectItem->getStatusIconData();
		if (status != nullptr)
		{
			auto qtImageProvider = dynamic_cast< QtImageProvider * >(
				qtFramework_->qmlEngine()->imageProvider( QtImageProvider::providerId() ) );
			if (qtImageProvider != nullptr)
			{
				return qtImageProvider->encodeImage( status );
			}
		}
	}

	return QVariant( QVariant::Invalid );
}

bool AssetItemExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	return false;
}

void AssetItemExtension::onDataChanged( const QModelIndex &index, int role, const QVariant &value )
{
	auto model = index.model();
	assert( model != nullptr );

	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return;
	}

	if (roleId == StatusIconRole::roleId_ ||
		roleId == TypeIconRole::roleId_ ||
		roleId == SizeRole::roleId_ ||
		roleId == CreatedTimeRole::roleId_ ||
		roleId == ModifiedTimeRole::roleId_ ||
		roleId == AccessedTimeRole::roleId_ ||
		roleId == IsDirectoryRole::roleId_ ||
		roleId == IsReadOnlyRole::roleId_ ||
		roleId == IsCompressedRole::roleId_)
	{
		QVector<int> roles;
		roles.append( role );
		emit const_cast< QAbstractItemModel * >( model )->dataChanged( index, index, roles );
	}
}
