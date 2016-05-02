#pragma once
#ifndef _REFLECTED_OBJECT_ITEM_NEW_HPP
#define _REFLECTED_OBJECT_ITEM_NEW_HPP

#include "reflected_tree_item_new.hpp"

#include <memory>


/**
 *	Create an item in a ReflectedTreeModel from an ObjectHandle.
 *	Can be a root or child item.
 */
class ReflectedObjectItemNew : public ReflectedTreeItemNew
{
public:

	/**
	 *	Construct a tree data model by reflecting over the given object.
	 *	@param object the root of the tree.
	 *		@warning the parent *must* correspond to the object.
	 *	@param parent the parent of this property.
	 */
	ReflectedObjectItemNew( IComponentContext & contextManager,
		const ObjectHandle & object,
		ReflectedTreeItemNew * parent = nullptr );
	virtual ~ReflectedObjectItemNew();

	// AbstractItem
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override;

	// ReflectedTreeItemNew
	virtual const ObjectHandle & getRootObject() const override;
	virtual const ObjectHandle & getObject() const override;
	virtual const IClassDefinition * getDefinition() const override;

	virtual ReflectedTreeItemNew * getChild( size_t index ) const override;
	virtual int rowCount() const override;

	virtual bool preSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;
	virtual bool postSetValue( const PropertyAccessor & accessor,
		const Variant & value ) override;

private:
	class Implementation;
	std::unique_ptr< Implementation > impl_;

	typedef std::function< bool( ReflectedTreeItemNew & ) > ReflectedItemCallback;
	void enumerateChildren( const ReflectedItemCallback & callback ) const;
};

#endif // _REFLECTED_OBJECT_ITEM_NEW_HPP
