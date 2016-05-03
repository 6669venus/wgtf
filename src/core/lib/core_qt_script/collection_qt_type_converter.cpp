#include "collection_qt_type_converter.hpp"

#include "core_data_model/collection_model_old.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "core_qt_common/models/qt_collection_model.hpp"


Q_DECLARE_METATYPE( ObjectHandle );


/**
 *	Convert a QVariant to a Collection.
 *	Note that o_variant will be referencing the collection in qVariant,
 *	does not perform a copy.
 *	The qVariant needs to be kept alive for the lifetime of o_variant.
 *	A CollectionHolder needs to be used to perform a copy.
 */
bool CollectionQtTypeConverterOld::toVariant( const QVariant & qVariant,
	Variant & o_variant ) const
{
	int typeId = qVariant.type();
	if (typeId == QVariant::UserType)
	{
		typeId = qVariant.userType();
	}

	if (typeId != qMetaTypeId< ObjectHandle >())
	{
		return false;
	}

	// QVariant stores collections in an ObjectHandle
	auto handle = qVariant.value< ObjectHandle >();
	if (!handle.isValid())
	{
		return false;
	}

	// Check if the ObjectHandle contains a CollectionModel
	auto pCollectionModel =
		handle.getBase< CollectionModelOld >();
	if (pCollectionModel == nullptr)
	{
		return false;
	}

	// Get Collection from CollectionModel
	o_variant = pCollectionModel->getSource();
	return true;
}


/**
 *	Convert a Collection to QVariant.
 *	Note that o_qVariant will be referencing the collection in variant,
 *	does not copy it.
 *	The variant needs to be kept alive to the lifetime of o_qVariant.
 *	A CollectionHolder needs to be used to perform a copy.
 */
bool CollectionQtTypeConverterOld::toQVariant(const Variant& variant,
	QVariant& o_qVariant, QObject* parent ) const
{
	if (!variant.typeIs< Collection >())
	{
		return false;
	}

	Collection value;
	if (!variant.tryCast( value ))
	{
		return false;
	}

	// Create a new collection model
	auto collectionModel = new CollectionModelOld();
	collectionModel->setSource( value );
	auto listModel = std::unique_ptr< IListModel >( collectionModel );

	// Give ownership to ObjectHandle
	ObjectHandle handle( std::move( listModel ) );

	// ObjectHandle can be converted to QVariant
	o_qVariant = QVariant::fromValue< ObjectHandle >( handle );
	return true;
}


/**
*	Convert a QVariant to a Collection.
*	Note that o_variant will be referencing the collection in qVariant,
*	does not perform a copy.
*	The qVariant needs to be kept alive for the lifetime of o_variant.
*	A CollectionHolder needs to be used to perform a copy.
*/
bool CollectionQtTypeConverter::toVariant(const QVariant & qVariant,
	Variant & o_variant) const
{
	int typeId = qVariant.type();
	if (typeId == QVariant::UserType)
	{
		typeId = qVariant.userType();
	}

	if (!qVariant.canConvert< QtCollectionModel* >())
	{
		return false;
	}

	auto model = qVariant.value< QtCollectionModel * >();
	o_variant = ObjectHandle(&model->source());
	return true;
}

/**
*	Convert a Collection to QVariant.
*	Note that o_qVariant will be referencing the collection in variant,
*	does not copy it.
*	The variant needs to be kept alive to the lifetime of o_qVariant.
*	A CollectionHolder needs to be used to perform a copy.
*/
bool CollectionQtTypeConverter::toQVariant(const Variant& variant,
	QVariant& o_qVariant, QObject* parent) const
{
	if ( !variant.typeIs< Collection >() )
	{
		return false;
	}

	Collection value;
	if ( !variant.tryCast( value ) )
	{
		return false;
	}

	// Create a new collection model
	auto collectionModel = new CollectionModel();
	collectionModel->setSource( value );

	auto model = new QtCollectionModel( *collectionModel );
	model->setParent(parent);
	o_qVariant = QVariant::fromValue( model );

	return true;
}