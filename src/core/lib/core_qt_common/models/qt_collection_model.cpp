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

	const auto foundItr = collection.find( variantKey );
	if (foundItr == collection.cend())
	{
		return nullptr;
	}

	int row = 0;
	for (auto itr = collection.cbegin(); itr != foundItr; ++itr)
	{
		++row;
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

	const auto variantKey = QtHelpers::toVariant( key );
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

	const auto variantKey = QtHelpers::toVariant( key );
	int row = -1;
	const auto isRow = variantKey.tryCast< int >( row );
	if (!isRow)
	{
		return false;
	}

	return QtItemModel::removeRow( row );
}
