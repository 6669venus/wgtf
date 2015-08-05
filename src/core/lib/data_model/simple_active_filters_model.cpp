#include "simple_active_filters_model.hpp"
#include "data_model/generic_list.hpp"
#include "i_item_role.hpp"

//------------------------------------------------------------------------------

struct SimpleActiveFiltersModel::Implementation
{
	Implementation( SimpleActiveFiltersModel& self );

	SimpleActiveFiltersModel& self_;
	GenericList filters_;
	std::string stringValue_;
	int removedIndex_;
	int selectedFilterIndex_;

	void addFilter( const char* text );
	const char* generateStringValue();
};

SimpleActiveFiltersModel::Implementation::Implementation( 
	SimpleActiveFiltersModel& self )
: self_( self )
, stringValue_( "" )
, removedIndex_( -1 )
, selectedFilterIndex_( -1 )
{
}

void SimpleActiveFiltersModel::Implementation::addFilter( const char* text )
{
	filters_.push_back( text );
}

//gnelsontodo - remove this func
const char* SimpleActiveFiltersModel::Implementation::generateStringValue()
{	
	stringValue_.clear();

	int iteration = 0;

	for (auto filterItr = filters_.begin(); 
		 filterItr != filters_.end();
		 ++filterItr)
	{
		auto tempObjHandle = ObjectHandle( *filterItr );
		auto tempItem = tempObjHandle.getBase<GenericListItem>();
		Variant variant = tempItem->getData( 0, ValueRole::roleId_ );

		if (variant.typeIs< const char * >() ||
			variant.typeIs< std::string >())
		{
			std::string value;
			if (variant.tryCast( value ))
			{
				if (iteration != 0)
				{
					stringValue_ += " ";
				}

				stringValue_ += value;
				++iteration;
			}
		}
	}

	return stringValue_.c_str();
}

//------------------------------------------------------------------------------

SimpleActiveFiltersModel::SimpleActiveFiltersModel()
: IActiveFiltersModel()
, impl_( new Implementation( *this ) )
{
}

SimpleActiveFiltersModel::~SimpleActiveFiltersModel()
{
}

ObjectHandle SimpleActiveFiltersModel::getFilters() const
{
	return impl_->filters_;
}

ObjectHandle SimpleActiveFiltersModel::getSavedFilters() const
{
	//TODO
	return nullptr;
}

const char* SimpleActiveFiltersModel::getStringValue() const
{
	return impl_->generateStringValue();
}

const int & SimpleActiveFiltersModel::removeFilter() const
{
	return impl_->removedIndex_;
}

void SimpleActiveFiltersModel::removeFilter( const int & index )
{
	impl_->removedIndex_ = index;
	if (impl_->removedIndex_ == -1)
	{
		return;
	}
		
	ObjectHandle selectedItem = impl_->filters_[ impl_->removedIndex_ ];

	for (auto filterItr = impl_->filters_.begin(); 
		 filterItr != impl_->filters_.end(); 
		 ++filterItr)
	{
		ObjectHandle tempObjHandle = ObjectHandle( *filterItr );
		if (tempObjHandle == selectedItem)
		{
			impl_->filters_.erase( filterItr );
			break;
		}
	}
}

const int & SimpleActiveFiltersModel::currentSelectedFilter() const
{
	return impl_->selectedFilterIndex_;
}

void SimpleActiveFiltersModel::currentSelectedFilter( const int & index )
{
	impl_->selectedFilterIndex_ = index;
}

bool SimpleActiveFiltersModel::clearFilters() const
{
	impl_->filters_.clear();
	return true;
}

const std::string & SimpleActiveFiltersModel::addFilter() const
{
	// Temporary until reflected methods are available.
	return tempString_;
}

void SimpleActiveFiltersModel::addFilter( const std::string & text )
{
	impl_->addFilter( text.c_str() );
}

const std::string & SimpleActiveFiltersModel::saveFilters() const
{
	// Temporary until reflected methods are available.
	return tempString_;
}

void SimpleActiveFiltersModel::saveFilters( const std::string & filename )
{
	//TODO
}

const std::string & SimpleActiveFiltersModel::loadFilters() const
{
	// Temporary until reflected methods are available.
	return tempString_;
}

void SimpleActiveFiltersModel::loadFilters( const std::string & filename )
{
	//TODO
}