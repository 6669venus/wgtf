#ifndef COLLECTION_MODEL_HPP
#define COLLECTION_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "core_variant/collection.hpp"
#include <memory>

/**
 *	Data model for displaying a Collection as a list.
 */
class CollectionModel
	: public AbstractListModel
{
public:
	CollectionModel();
	virtual ~CollectionModel();

	void setSource(Collection & collection);
	const Collection & getSource() const;
	Collection & getSource();

	// AbstractListModel
	virtual AbstractItem * item(int index) const override;
	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

protected:
	Collection collection_;
	mutable std::vector< std::unique_ptr< AbstractItem > > items_;
};


#endif // COLLECTION_LIST_MODEL_HPP
