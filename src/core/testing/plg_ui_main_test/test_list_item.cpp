#include "test_list_item.hpp"
#include "core_data_model/i_item_role.hpp"

enum class TestListItemType
{
	STRING,
	NUMBER
};

struct TestListItem::Implementation
{
	Implementation(
		TestListItem& self,
		const std::string& name,
		const std::string& value );
	Implementation(
		TestListItem& self,
		const std::string& name,
		size_t value );
	~Implementation();

	TestListItem& self_;
	TestListItemType type_;
	std::string name_;
	std::string string_;
	size_t number_;
};


TestListItem::Implementation::Implementation(
	TestListItem& self,
	const std::string& name,
	const std::string& value )
	: self_( self )
	, name_( name )
	, string_( value )
	, number_( 0 )
	, type_( TestListItemType::STRING )
{
}


TestListItem::Implementation::Implementation(
	TestListItem& self,
	const std::string& name,
	size_t value )
	: self_( self )
	, name_( name )
	, number_( value )
	, type_( TestListItemType::NUMBER )
{
}


TestListItem::Implementation::~Implementation()
{
}


TestListItem::TestListItem( const char* name, const char* value )
	: impl_( new Implementation( *this, name, value ) )
{
}


TestListItem::TestListItem( const char* name, const size_t value )
	: impl_( new Implementation( *this, name, value ) )
{
}


TestListItem::TestListItem( const TestListItem& rhs )
{
	Implementation& from = *rhs.impl_.get();

	if (from.type_ == TestListItemType::STRING)
	{
		impl_.reset( new Implementation( *this, from.name_, from.string_ ) );
	}
	else
	{
		impl_.reset( new Implementation( *this, from.name_, from.number_ ) );
	}
}


TestListItem::~TestListItem()
{
}


TestListItem& TestListItem::operator=( const TestListItem& rhs )
{
	if (this != &rhs)
	{
		Implementation& from = *rhs.impl_.get();

		if (from.type_ == TestListItemType::STRING)
		{
			impl_.reset( new Implementation(
				*this, from.name_, from.string_ ) );
		}
		else
		{
			impl_.reset( new Implementation(
				*this, from.name_, from.number_ ) );
		}
	}

	return *this;
}


int TestListItem::columnCount() const
{
	return 2;
}


const char* TestListItem::getDisplayText( int column ) const
{
	if (column == 0)
	{
		return impl_->name_.c_str();
	}
	else if (column == 1)
	{
		return "Value";
	}

	return nullptr;
}


ThumbnailData TestListItem::getThumbnail( int column ) const
{
	return nullptr;
}


Variant TestListItem::getData( int column, size_t roleId ) const
{
	if (roleId == ValueTypeRole::roleId_)
	{
		if (column == 0 || impl_->type_ == TestListItemType::STRING)
		{
			return typeid( std::string ).name();
		}
		else
		{
			return typeid( size_t ).name();
		}
	}
	else if (roleId == ValueRole::roleId_)
	{
		if (column == 0)
		{
			return impl_->name_;
		}
		else if (impl_->type_ == TestListItemType::STRING)
		{
			return impl_->string_;
		}
		else
		{
			return impl_->number_;
		}
	}

	return Variant();
}


bool TestListItem::setData( int column, size_t roleId, const Variant& data )
{
	if (column == 1)
	{
		std::string string;
		size_t number;

		if (data.tryCast( string ))
		{
			impl_->string_ = string;
			impl_->number_ = 0;
			impl_->type_ = TestListItemType::STRING;
		}
		else if (data.tryCast( number ))
		{
			impl_->string_ = "";
			impl_->number_ = number;
			impl_->type_ = TestListItemType::NUMBER;
		}
	}

	return false;
}
