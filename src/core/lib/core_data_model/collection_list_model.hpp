#ifndef COLLECTION_LIST_MODEL_HPP
#define COLLECTION_LIST_MODEL_HPP

#include "core_data_model/i_list_model.hpp"
#include "core_data_model/reflection/reflected_object_item.hpp"

#include <memory>

class ObjectHandle;

/**
 *	Data model for displaying a Collection as a list.
 */
class CollectionListModel
	: public IListModel
{
public:
	class Impl;
	CollectionListModel( const ObjectHandle& object, const char* path );
	virtual ~CollectionListModel();

	// IListModel
	virtual size_t index( const IItem* item ) const override;

	virtual bool empty() const override;
	virtual size_t size() const override;

private:
	CollectionListModel( const CollectionListModel& other );
	CollectionListModel( CollectionListModel&& other );
	CollectionListModel& operator=( const CollectionListModel& other );
	CollectionListModel& operator&&( CollectionListModel&& other );
	
	std::unique_ptr< Impl > impl_;
	
};

#endif // COLLECTION_LIST_MODEL_HPP
