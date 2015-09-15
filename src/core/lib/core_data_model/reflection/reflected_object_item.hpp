#ifndef REFLECTED_OBJECT_ITEM_HPP
#define REFLECTED_OBJECT_ITEM_HPP

#include "reflected_item.hpp"
#include "core_reflection_utils/reflectedpropertyrootobject_setter.hpp"
#include "core_reflection/object_handle.hpp"

class ReflectedObjectItem : public ReflectedItem
{
public:
	ReflectedObjectItem( const ObjectHandle & object, ReflectedItem * parent = nullptr );
	virtual ~ReflectedObjectItem() {}

	// ReflectedItem
	const ObjectHandle & getObject() const override { return parent_ ? parent_->getObject() : object_; }
	const IClassDefinition * getDefinition() const override;

	// IItem
	const char * getDisplayText( int column ) const override;
	Variant getData( int column, size_t roleId ) const override;
	bool setData(int column, size_t roleId, const Variant & data) override;

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
	void onPreDataChanged(const ReflectedPropertyRootObjectSetter* sender,
		const ReflectedPropertyRootObjectSetter::PreDataChangedArgs& args);
	void onPostDataChanged(const ReflectedPropertyRootObjectSetter* sender,
		const ReflectedPropertyRootObjectSetter::PostDataChangedArgs& args);

	ObjectHandle object_;
	mutable std::string displayName_;
	mutable std::vector< std::unique_ptr< ReflectedItem > > children_;
	std::unique_ptr< ReflectedPropertyRootObjectSetter > rootObjectSetter_;
};

#endif //REFLECTED_OBJECT_ITEM_HPP
