#ifndef REFLECTED_PROPERTY_ITEM_HPP
#define REFLECTED_PROPERTY_ITEM_HPP

#include "core_data_model/i_list_model.hpp"
#include "reflected_item.hpp"
#include "core_reflection/property_accessor.hpp"

class IBaseProperty;


/**
 *	Class for adding child reflected properties to a tree.
 *	Via a parent & string or via a parent & child property pair.
 *	E.g. "parent.child" or "parent.child[0]" or (*parent, *child)
 */
class ReflectedPropertyItem : public ReflectedItem
{
public:
	ReflectedPropertyItem( IBaseProperty * property, ReflectedItem * parent );
	ReflectedPropertyItem( const std::string & propertyName,
		std::string && displayName,
		ReflectedItem * parent );
	ReflectedPropertyItem( const char * propertyName, const ObjectHandle & object );
	virtual ~ReflectedPropertyItem();

	// ReflectedItem
	const ObjectHandle & getObject() const override
	{
		if (parent_ != nullptr)
		{
			return parent_->getObject();
		}
		else
		{
			assert( *pObject_ != nullptr );
			return *pObject_;
		}
	}

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
	const ObjectHandle * const pObject_;
};

#endif