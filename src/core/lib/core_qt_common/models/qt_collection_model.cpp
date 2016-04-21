#include "qt_collection_model.hpp"
#include "core_data_model/collection_model.hpp"


QtListModel::QtListModel( CollectionModel & source ) 
	: QtListModel( source ) 
{
}


const CollectionModel & QtListModel::source() const
{
	return static_cast< const CollectionModel & >( QtItemModel::source() );
}


//QObject * QtListModel::item( const QVariant & key ) const
//{
//	const auto variantKey = QtHelpers::toVariant( key );
//	const auto & collectionModel = this->source();
//	const auto & collection = collectionModel.getSource();
//	if (variantKey.type

//	return QtItemModel::item( row, 0, nullptr );
//}


//bool QtListModel::insertItem( const QVariant & key )
//{
//	return QtItemModel::insertRow( row );
//}


//bool QtListModel::removeItem( const QVariant & key )
//{
//	return QtItemModel::removeRow( row );
//}
