#pragma once
#ifndef _REFLECTED_TREE_ITEM_NEW_HPP
#define _REFLECTED_TREE_ITEM_NEW_HPP

#include "core_data_model/abstract_item.hpp"
#include "core_dependency_system/di_ref.hpp"

class AbstractTreeModel;

class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;

class IClassDefinition;
class IDefinitionManager;
class IReflectionController;
class ObjectHandle;
class PropertyAccessor;

/**
 *	Base class for adding a reflected item to a tree.
 */
class ReflectedTreeItemNew : public AbstractTreeItem
{
public:
	ReflectedTreeItemNew( IComponentContext & contextManager,
		ReflectedTreeItemNew * parent,
		const char * path ) ;
	ReflectedTreeItemNew( IComponentContext & contextManager,
		ReflectedTreeItemNew * parent,
		const std::string & path );
	virtual ~ReflectedTreeItemNew();

	virtual const ObjectHandle & getRootObject() const = 0;
	virtual const ObjectHandle & getObject() const = 0;
	virtual const IClassDefinition * getDefinition() const;

	const std::string & getPath() const;
	IReflectionController * getController() const;
	IDefinitionManager * getDefinitionManager() const;

	// AbstractTreeItem 
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override;

	virtual bool preSetValue( const PropertyAccessor & accessor, const Variant & value ) = 0;
	virtual bool postSetValue( const PropertyAccessor & accessor, const Variant & value ) = 0;

	// TODO remove this from the generic model and move into a filter
	bool hidden() const;
	void hidden( bool value );

	//AbstractTreeModel * getModel() const;

	ReflectedTreeItemNew * getParent() const;
	virtual ReflectedTreeItemNew * getChild( size_t index ) const = 0;

	virtual int size() const = 0;

protected:
	ReflectedTreeItemNew * parent_;
	std::string path_;
	DIRef< IReflectionController > controller_;
	DIRef< IDefinitionManager > definitionManager_;

	typedef std::function< bool( const IBasePropertyPtr &, const std::string & ) >
		PropertyCallback;
	bool enumerateVisibleProperties( const PropertyCallback & callback ) const;

private:
	static bool enumerateVisibleProperties( ObjectHandle object,
		const IDefinitionManager & definitionManager,
		const std::string & inPlacePath,
		const PropertyCallback & callback );

	AbstractTreeModel * model_;
	bool hidden_;
};

#endif //_REFLECTED_TREE_ITEM_NEW_HPP