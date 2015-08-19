#ifndef REFLECTED_PROPERTY_ITEM_HPP
#define REFLECTED_PROPERTY_ITEM_HPP

#include "core_data_model/value_change_notifier.hpp"
#include "reflected_item.hpp"
#include "core_reflection/property_accessor.hpp"

class IBaseProperty;
class IComboBoxListModel;

class ReflectedPropertyItem : public ReflectedItem
{
public:
	ReflectedPropertyItem( IBaseProperty * property, ReflectedItem * parent );
	ReflectedPropertyItem( const std::string & propertyName, ReflectedItem * parent );
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

	void onPostDataChanged(const IValueChangeNotifier* sender,
		const IValueChangeNotifier::PostDataChangedArgs& args);

private:
	std::string displayName_;
	mutable std::vector< std::unique_ptr< ReflectedItem > > children_;
	mutable std::unique_ptr< IComboBoxListModel > listModel_;

	// current combbox index
	mutable ValueChangeNotifier< int >	currentIndex_;
};

#endif