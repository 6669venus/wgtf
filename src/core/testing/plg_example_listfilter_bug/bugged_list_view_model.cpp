#include "bugged_list_view_model.hpp"
#include "bugged_list_view_model.mpp"
#include "core_data_model/simple_active_filters_model.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"

#include <vector>
#include <string>
#include <stdlib.h> // for srand/rand
#include <time.h> // for time
#include <Windows.h> // for Sleep

//------------------------------------------------------------------------------
// Implementation (PIMPL)
//------------------------------------------------------------------------------

struct BuggedListViewModel::Implementation
{
	Implementation( BuggedListViewModel& self );
	void init( IDefinitionManager & defManager );
	void populateList();

	BuggedListViewModel& self_;
	IDefinitionManager* defManager_;
	ObjectHandle simpleActiveFiltersModel_;
	GenericList sampleDataToFilter_;
	std::vector<std::string> sourceData_;
};

BuggedListViewModel::Implementation::Implementation( 
	BuggedListViewModel& self )
	: self_( self )
	, defManager_( nullptr )
	, simpleActiveFiltersModel_( nullptr )
{
	// Create some test source that would mimic the resource files.
	sourceData_.push_back( "bunny" );
	sourceData_.push_back( "frog" );
	sourceData_.push_back( "pig" );
	sourceData_.push_back( "dog" );
	sourceData_.push_back( "chocobo" );
	sourceData_.push_back( "froglok" );
	sourceData_.push_back( "rabbit" );
	sourceData_.push_back( "bear" );
	sourceData_.push_back( "griffin" );
	sourceData_.push_back( "dragon" );
	sourceData_.push_back( "cat" );
	sourceData_.push_back( "lion" );
	sourceData_.push_back( "cheetah" );
	sourceData_.push_back( "rhino" );
	sourceData_.push_back( "elephant" );
	sourceData_.push_back( "unicorn" );
	sourceData_.push_back( "horse" );
	sourceData_.push_back( "cow" );
	sourceData_.push_back( "duck" );
	sourceData_.push_back( "goose" );
	sourceData_.push_back( "rat" );
	sourceData_.push_back( "field mouse" );
	sourceData_.push_back( "bird" );
	sourceData_.push_back( "chicken" );
	sourceData_.push_back( "dinosaur" );
	sourceData_.push_back( "squirrel!!!!!!!!" );
	sourceData_.push_back( "panda" );
	sourceData_.push_back( "red panda" );
	sourceData_.push_back( "snail" );
	sourceData_.push_back( "shark" );
	sourceData_.push_back( "squid" );
	sourceData_.push_back( "hammerhead shark" );
}

void BuggedListViewModel::Implementation::init( 
	IDefinitionManager & defManager )
{
	defManager_ = &defManager;
	auto def = defManager_->getDefinition< IActiveFiltersModel >();
	auto impl = std::unique_ptr< IActiveFiltersModel >(
		new SimpleActiveFiltersModel );

	simpleActiveFiltersModel_ = ObjectHandle( std::move( impl ), def );

	// Initial data population
	populateList();
}

void BuggedListViewModel::Implementation::populateList()
{
	// Clear the list
	//sampleDataToFilter_.clear(); // Siegi suggested not using clear(). Doesn't appear to change outcome.
	for (auto itr = sampleDataToFilter_.begin(); itr != sampleDataToFilter_.end(); )
	{
		itr = sampleDataToFilter_.erase( itr );
	}

	Sleep( 1000 );

	// Populate the list with new data
	for (int i = 0; i < 4; ++i)
	{
		int temp = rand() % 31;
		std::unique_ptr<ExampleListItem> item( new ExampleListItem( sourceData_[temp] ) );
		sampleDataToFilter_.emplace_back( item.release() );
	}
}

//------------------------------------------------------------------------------
// View Model
// Data passed to the QML panel to be used by the control(s).
//------------------------------------------------------------------------------

BuggedListViewModel::BuggedListViewModel() 
	: impl_( new Implementation( *this ) )
{
	srand( static_cast<unsigned int>( time(NULL)  ) );
}

BuggedListViewModel::~BuggedListViewModel()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}
	
void BuggedListViewModel::init( IDefinitionManager & defManager )
{
	impl_->init( defManager );
}

ObjectHandle BuggedListViewModel::getSimpleActiveFiltersModel() const
{
	return impl_->simpleActiveFiltersModel_;
}

ObjectHandle BuggedListViewModel::getSampleDataToFilter() const 
{
	return impl_->sampleDataToFilter_;
}

bool BuggedListViewModel::generateNewList() const
{
	impl_->populateList();
	return true;
}

//------------------------------------------------------------------------------
// Bugged List Item
//------------------------------------------------------------------------------

ExampleListItem::ExampleListItem( const Variant& value )
	: GenericListItem( value )
	, name_( "Unknown" )
{
	if (value.typeIs< const char * >() ||
		value.typeIs< std::string >())
	{
		value.tryCast( name_ );
	}
}

ExampleListItem::ExampleListItem( Variant&& value )
	: GenericListItem( std::forward<Variant&&>( value ) )
	, name_( "Unknown" )
{
	const Variant & Value = this->value<const Variant &>();

	if (Value.typeIs< const char * >() ||
		Value.typeIs< std::string >())
	{
		Value.tryCast( name_ );
	}
}

Variant ExampleListItem::getData( int column, size_t roleId ) const
{
	if (column == 0 && roleId == ValueRole::roleId_)
	{
		return name_;
	}

	return Variant();
}

const char * ExampleListItem::getDisplayText( int column ) const 
{ 
	return name_.c_str();
}
