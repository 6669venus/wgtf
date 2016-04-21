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
	int row = -1;
	const auto isRow = variantKey.tryCast< int >( row );
	if (!isRow)
	{
		return false;
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
