#ifndef REFLECTED_ITEM_HPP
#define REFLECTED_ITEM_HPP

#include "core_data_model/generic_tree_item.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/collection.hpp"

class PropertyAccessor;
class IReflectionController;
class ObjectHandle;
class IDefinitionManager;

class ReflectedItem : public GenericTreeItem
{
public:
	ReflectedItem( ReflectedItem * parent, const char * path ) 
		: parent_( parent )
		, path_( path )
		, controller_( nullptr )
		, definitionManager_( nullptr ) {}
	ReflectedItem( ReflectedItem * parent, const std::string & path ) 
		: parent_( parent )
		, path_( path )
		, controller_( nullptr )
		, definitionManager_( nullptr ) {}
	virtual ~ReflectedItem() {}

	virtual const ObjectHandle & getObject() const = 0;
	virtual const IClassDefinition * getDefinition() const;

	const std::string & getPath() const { return path_; }
	IReflectionController * getController() const;
	void setController( IReflectionController * controller );
	IDefinitionManager * getDefinitionManager() const;
	void setDefinitionManager( IDefinitionManager * definitionManager );

	// IItem
	ThumbnailData getThumbnail( int column ) const { return nullptr; }
	Variant getData( int column, size_t roleId ) const { return Variant(); }
	bool setData( int column, size_t roleId, const Variant & data ) { return false; }

	// GenericTreeItem
	GenericTreeItem * getParent() const { return parent_; }

	virtual bool preSetValue( const PropertyAccessor & accessor, const Variant & value ) = 0;
	virtual bool postSetValue( const PropertyAccessor & accessor, const Variant & value ) = 0;
	virtual bool preItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & pos, size_t count ) = 0;
	virtual bool postItemsInserted( const PropertyAccessor & accessor, 
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) = 0;
	virtual bool preItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & begin, const Collection::ConstIterator & end ) = 0;
	virtual bool postItemsRemoved( const PropertyAccessor & accessor,
		const Collection::ConstIterator & pos, size_t count ) = 0;

protected:
	ReflectedItem * parent_;
	std::string path_;
	IReflectionController * controller_;
	IDefinitionManager * definitionManager_;
};

#endif //REFLECTED_ITEM_HPP