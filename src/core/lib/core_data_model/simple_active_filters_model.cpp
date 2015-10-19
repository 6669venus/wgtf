#include "simple_active_filters_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "i_item_role.hpp"

//------------------------------------------------------------------------------

struct SimpleActiveFiltersModel::Implementation
{
	Implementation( SimpleActiveFiltersModel& self, IDefinitionManager & definitionManager );

	SimpleActiveFiltersModel& self_;
	IDefinitionManager& definitionManager_;
	VariantList filters_;
	std::string stringValue_;
	int removedIndex_;
	int selectedFilterIndex_;

	void addFilter( const char* text );
	const char* generateStringValue();
};

SimpleActiveFiltersModel::Implementation::Implementation( 
	SimpleActiveFiltersModel& self,
	IDefinitionManager & definitionManager )
: self_( self )
, definitionManager_( definitionManager )
, stringValue_( "" )
, removedIndex_( -1 )
, selectedFilterIndex_( -1 )
{
}

void SimpleActiveFiltersModel::Implementation::addFilter( const char* text )
{
	auto filterTerm = definitionManager_.create< ActiveFilterTerm >();
	filterTerm->setValue( text );
	filters_.push_back( filterTerm );
}

const char* SimpleActiveFiltersModel::Implementation::generateStringValue()
{	
	stringValue_.clear();

	int iteration = 0;

	for (auto filterItr = filters_.begin(); 
		 filterItr != filters_.end();
		 ++filterItr)
	{
		Variant variant = *filterItr;

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

SimpleActiveFiltersModel::SimpleActiveFiltersModel( IDefinitionManager & definitionManager )
: IActiveFiltersModel()
, impl_( new Implementation( *this, definitionManager ) )
{
}

SimpleActiveFiltersModel::~SimpleActiveFiltersModel()
{
}

IListModel * SimpleActiveFiltersModel::getFilters() const
{
	return &impl_->filters_;
}

ObjectHandle SimpleActiveFiltersModel::getSavedFilters() const
{
	// TODO
	// JIRA: http://jira.bigworldtech.com/browse/NGT-861
	return nullptr;
}

void SimpleActiveFiltersModel::removeFilter( int index )
{
	impl_->removedIndex_ = index;
	if (impl_->removedIndex_ == -1 || index >= (int) impl_->filters_.size())
	{
		return;
	}

	int tracker = 0;
	for (auto filterItr = impl_->filters_.begin(); filterItr != impl_->filters_.end(); ++filterItr)
	{
		if (tracker == impl_->removedIndex_)
		{
			impl_->filters_.erase( filterItr );
			break;
		}

		++tracker;
	}
}

void SimpleActiveFiltersModel::selectedFilter( int index )
{
	impl_->selectedFilterIndex_ = index;
}

void SimpleActiveFiltersModel::clearFilters()
{
	impl_->filters_.clear();
}

void SimpleActiveFiltersModel::addFilter( std::string text )
{
	impl_->addFilter( text.c_str() );
}

void SimpleActiveFiltersModel::saveFilters( std::string filename )
{
	// TODO
	// JIRA: http://jira.bigworldtech.com/browse/NGT-861
}

void SimpleActiveFiltersModel::loadFilters( std::string filename )
{
	// TODO
	// JIRA: http://jira.bigworldtech.com/browse/NGT-861
}