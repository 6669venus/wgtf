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


CollectionModel & QtCollectionModel::source()
{
	return static_cast< CollectionModel & >( QtItemModel::source() );
}


bool QtCollectionModel::insertRows( int row,
	int count,
	const QModelIndex & parent )
{
	const auto & collectionModel = this->source();
	const auto & collection = collectionModel.getSource();

	// Insert/remove by row disabled for mapping types
	if (collection.isMapping())
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert index directly to key
	QVariant key( row );
	return this->insertItem( key );
}


bool QtCollectionModel::removeRows( int row,
	int count,
	const QModelIndex & parent )
{
	const auto & collectionModel = this->source();
	const auto & collection = collectionModel.getSource();

	// Insert/remove by row disabled for mapping types
	if (collection.isMapping())
	{
		return false;
	}

	// Since this is an index-able collection
	// Convert index directly to key
	QVariant key( row );
	return this->removeItem( key );
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
	auto & collectionModel = this->source();
	auto & collection = collectionModel.getSource();

	const auto variantKey = QtHelpers::toVariant( key );
	const auto insertItr = collection.insert( variantKey );
	return (insertItr != collection.end());
}


bool QtCollectionModel::removeItem( const QVariant & key )
{
	auto & collectionModel = this->source();
	auto & collection = collectionModel.getSource();

	const auto variantKey = QtHelpers::toVariant( key );
	const auto erasedCount = collection.erase( variantKey );
	return (erasedCount > 0);
}
