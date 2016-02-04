#ifndef REFLECTED_PROPERTY_ITEM_HPP
#define REFLECTED_PROPERTY_ITEM_HPP

#include "core_data_model/i_list_model.hpp"
#include "reflected_item.hpp"
#include "core_reflection/property_accessor.hpp"

class IBaseProperty;


/**
 *	Create an item in a ReflectedTreeModel from an IBaseProperty on an ObjectHandle.
 *	Must be a child item.
 *	Via a parent+string or via a parent+child property pair.
 *	E.g. strings "parent.child" or "parent.child[0]"
 *		or pair (*parent, *child)
 */
class ReflectedPropertyItem : public ReflectedItem
{
public:

	/**
	 *	Construct a data model by reflecting over the given property.
	 *	
	 *	@pre parent must not be null.
	 *	
	 *	@param property the property to use for the data for this item.
	 *	@param parent the parent of this property.
	 *		Cannot be null.
	 *		@warning the parent *must* correspond to the property.
	 */
	ReflectedPropertyItem( IBaseProperty * property, ReflectedItem * parent );

	/**
	 *	Construct a data model by reflecting over the given property.
	 *	
	 *	@pre parent must not be null.
	 *	
	 *	@param propertyName the name of the property on the parent.
	 *	@param displayName the name used for display in the UI.
	 *	@param parent the parent of this property.
	 *		Cannot be null.
	 */
	ReflectedPropertyItem( const std::string & propertyName,
		std::string displayName,
		ReflectedItem * parent );
	virtual ~ReflectedPropertyItem();

	// ReflectedItem
	const ObjectHandle & getObject() const override { return parent_->getObject(); }

	// IItem
	const char * getDisplayText( int column ) const override;
	ThumbnailData getThumbnail( int column ) const override;
	Variant getData( int column, size_t roleId ) const override;
	bool setData( int column, size_t roleId, const Variant & data ) override;

	// GenericTreeItem
	GenericTreeItem * getChild( size_t index ) const override;

	bool empty() const override;
	size_t size() const override;

	bool preSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override;
	bool postSetValue(
		const PropertyAccessor & accessor, const Variant & value ) override;
	bool preItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & pos,
		size_t count ) override;
	bool postItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & begin,
		const Collection::ConstIterator & end ) override;
	bool preItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & begin,
		const Collection::ConstIterator & end ) override;
	bool postItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & pos,
		size_t count ) override;

private:
	std::string displayName_;
	mutable std::vector< std::unique_ptr< ReflectedItem > > children_;
};

#endif
