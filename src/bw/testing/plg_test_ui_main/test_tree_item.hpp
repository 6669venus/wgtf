#ifndef TEST_TREE_ITEM_HPP
#define TEST_TREE_ITEM_HPP

#include "data_model/i_item.hpp"

#include <memory>

class TestTreeItem: public IItem
{
public:
	TestTreeItem( const char* name, const IItem* parent );
	TestTreeItem( const TestTreeItem& rhs );
	virtual ~TestTreeItem();

	TestTreeItem& operator=( const TestTreeItem& rhs );

	const IItem* getParent() const;
	virtual int columnCount() const;
	virtual const char* getDisplayText( int column ) const;
	virtual ThumbnailData getThumbnail( int column ) const;
	virtual Variant getData( int column, size_t roleId ) const;
	virtual bool setData( int column, size_t roleId, const Variant& data );

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif
