#ifndef REFLECTED_GROUP_ITEM_HPP
#define REFLECTED_GROUP_ITEM_HPP

#include "reflected_item.hpp"

class MetaGroupObj;

class ReflectedGroupItem : public ReflectedItem
{
public:
	ReflectedGroupItem( const MetaGroupObj * groupObj, ReflectedItem * parent );
	virtual ~ReflectedGroupItem() {}

	// ReflectedItem
	const ObjectHandle & getRootObject() const override{ return parent_->getRootObject(); }
    const ObjectHandle & getObject() const override { return parent_->getObject(); }
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

private:
	typedef std::vector< Variant > Variants;

	void getChildren(ObjectHandle obj, std::string &childPath, Variants &childValues_) const;
	bool isSameGroup(const MetaGroupObj* group) const;

	const MetaGroupObj * groupObj_;
	std::string displayName_;
	mutable std::vector< std::unique_ptr< ReflectedItem > > children_;
};

#endif //REFLECTED_GROUP_ITEM_HPP