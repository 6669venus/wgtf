#pragma once
#ifndef _REFLECTED_PROPERTY_ITEM_NEW_HPP
#define _REFLECTED_PROPERTY_ITEM_NEW_HPP


#include "reflected_tree_item_new.hpp"

#include <memory>


/**
 *	Create an item in a ReflectedTreeModel from an IBaseProperty on an ObjectHandle.
 *	Must be a child item.
 *	Via a parent+string or via a parent+child property pair.
 *	E.g. strings "parent.child" or "parent.child[0]"
 *		or pair (*parent, *child)
 */
class ReflectedPropertyItemNew : public ReflectedTreeItemNew
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
	ReflectedPropertyItemNew( IComponentContext & contextManager,
		const IBasePropertyPtr & property,
		ReflectedTreeItemNew * parent,
		const std::string & inplacePath );

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
	ReflectedPropertyItemNew( const std::string & propertyName,
		std::string displayName,
		ReflectedTreeItemNew * parent );
	virtual ~ReflectedPropertyItemNew();

	// ReflectedTreeItemNew
	virtual const ObjectHandle & getRootObject() const override;
	virtual const ObjectHandle & getObject() const override;
	virtual ReflectedTreeItemNew * getChild( size_t index ) const override;

	// AbstractItem
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override;

	virtual size_t size() const override;

	virtual bool preSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;
	virtual bool postSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;
};

#endif // _REFLECTED_PROPERTY_ITEM_NEW_HPP

