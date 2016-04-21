#include "qt_collection_model.hpp"

#include "helpers/qt_helpers.hpp"

#include "core_data_model/collection_model.hpp"


QtCollectionModel::QtCollectionModel( CollectionModel & source ) 
	: QtListModel( source ) 
{
}


const CollectionModel & QtCollectionModel::source() const
{
	return static_cast< const CollectionModel & >( QtItemModel::source() );
}


QObject * QtCollectionModel::item( const QVariant & key ) const
{
	const auto & collectionModel = this->source();
	const auto & collection = collectionModel.getSource();

	// Check key types match
	const auto variantKey = QtHelpers::toVariant( key );
	const auto pMetaType = variantKey.type();
	if (pMetaType == nullptr)
	{
		return nullptr;
	}
	if (pMetaType->typeId() != collection.keyType())
	{
		return nullptr;
	}

	int row = 0;
	if (collection.isMapping())
	{
		// Key type may not be an index.
		// E.g. map[ "string key" ]
		// Iterate to convert key to an index.
		auto itr = collection.cbegin();
		for (; itr != collection.cend(); ++itr, ++row)
		{
			if (itr.key() == variantKey)
			{
				break;
			}
		}
		if (itr == collection.cend())
		{
			return nullptr;
		}
	}
	else
	{
		// Since this is an index-able collection
		// Convert key directly to index
		const auto isRow = variantKey.tryCast< int >( row );
		if (!isRow)
		{
			return nullptr;
		}
	}

	assert( QtItemModel::hasIndex( row, 0 ) );
	return QtItemModel::item( row, 0, nullptr );
}


bool QtCollectionModel::insertItem( const QVariant & key )
{
	const auto & collectionModel = this->source();
	const auto & collection = collectionModel.getSource();

	// Insert/remove disabled for mapping types
	// which may not be ordered
	if (collection.isMapping())
	{
		return false;
	}

	// Check key types match
	const auto variantKey = QtHelpers::toVariant( key );
	const auto pMetaType = variantKey.type();
	if (pMetaType == nullptr)
	{
		return false;
	}
	if (pMetaType->typeId() != collection.keyType())
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert key directly to index
	int row = -1;
	const auto isRow = variantKey.tryCast< int >( row );
	if (!isRow)
	{
		return false;
	}

	return QtItemModel::insertRow( row );
}


bool QtCollectionModel::removeItem( const QVariant & key )
{
	const auto & collectionModel = this->source();
	const auto & collection = collectionModel.getSource();

	// Insert/remove disabled for mapping types
	// which may not be ordered
	if (collection.isMapping())
	{
		return false;
	}

	// Check key types match
	const auto variantKey = QtHelpers::toVariant( key );
	const auto pMetaType = variantKey.type();
	if (pMetaType == nullptr)
	{
		return false;
	}
	if (pMetaType->typeId() != collection.keyType())
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert key directly to index
	int row = -1;
	const auto isRow = variantKey.tryCast< int >( row );
	if (!isRow)
	{
		return false;
	}

	return QtItemModel::removeRow( row );
}
