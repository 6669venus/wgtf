#ifndef TEST_LIST_ITEM_HPP
#define TEST_LIST_ITEM_HPP

#include "core_data_model/i_item.hpp"
#include "test_list_model.hpp"

#include <memory>

class TestListItem: public IItem
{
public:
	TestListItem( const char* name, const char* value );
	TestListItem( const char* name, const size_t value );
	TestListItem( const TestListItem& rhs );
	virtual ~TestListItem();

	TestListItem& operator=( const TestListItem& rhs );

	virtual int columnCount() const override;
	virtual const char* getDisplayText( int column ) const override;
	virtual ThumbnailData getThumbnail( int column ) const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif
