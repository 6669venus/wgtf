#include "simple_active_filters_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "i_item_role.hpp"
#include <sstream>

static const char * sActiveFiltersPropertyKey = "activefilters";

//------------------------------------------------------------------------------

struct SimpleActiveFiltersModel::Implementation
{
	Implementation( SimpleActiveFiltersModel& self, IDefinitionManager & definitionManager, 
		IUIFramework & uiFramework );

	SimpleActiveFiltersModel& self_;
	IDefinitionManager& definitionManager_;
	IUIFramework& uiFramework_;
	VariantList filterTerms_;
	VariantList savedFilters_;
	std::string stringValue_;
	int removedIndex_;

	void addFilter( const char* text );
	void addSavedFilter( const char* filterId );
	void generateStringValue();
};

SimpleActiveFiltersModel::Implementation::Implementation( 
	SimpleActiveFiltersModel& self,
	IDefinitionManager & definitionManager,
	IUIFramework & uiFramework)
: self_( self )
, definitionManager_( definitionManager )
, uiFramework_( uiFramework )
, stringValue_( "" )
, removedIndex_( -1 )
{
}

void SimpleActiveFiltersModel::Implementation::addFilter( const char* text )
{
	auto filterTerm = definitionManager_.create< ActiveFilterTerm >();
	filterTerm->setValue( text );
	filterTerms_.push_back( filterTerm );
}

void SimpleActiveFiltersModel::Implementation::addSavedFilter( const char* filterId )
{
	generateStringValue();

	auto savedFilter = definitionManager_.create< SavedActiveFilter >();
	
	savedFilter->setFilterId( filterId );
	savedFilter->setTerms( stringValue_ );

	savedFilters_.push_back( savedFilter );
}

void SimpleActiveFiltersModel::Implementation::generateStringValue()
{	
	stringValue_.clear();

	int iteration = 0;
	int maxSize = static_cast< int >( filterTerms_.size() );

	for (auto filterItr = filterTerms_.begin(); 
		 filterItr != filterTerms_.end();
		 ++filterItr)
	{
		Variant variant = *filterItr;
		ObjectHandle provider;
		if (variant.tryCast< ObjectHandle >( provider ))
		{
			auto term = provider.getBase< ActiveFilterTerm >();
			if (term == nullptr)
			{
				// Generate an empty string for the filter terms to make it obvious there is an issue, but
				// without crashing the application.
				stringValue_.clear();
				return;
			}

			if (iteration != 0 && iteration < maxSize)
			{
				stringValue_ += ",";
			}

			stringValue_ += term->getValue();
			++iteration;
		}
	}
}

//------------------------------------------------------------------------------

SimpleActiveFiltersModel::SimpleActiveFiltersModel( IDefinitionManager & definitionManager, IUIFramework & uiFramework )
: IActiveFiltersModel()
, impl_( new Implementation( *this, definitionManager, uiFramework ) )
{
}

SimpleActiveFiltersModel::~SimpleActiveFiltersModel()
{
}

IListModel * SimpleActiveFiltersModel::getCurrentFilterTerms() const
{
	return &impl_->filterTerms_;
}

void SimpleActiveFiltersModel::removeFilterTerm( int index )
{
	impl_->removedIndex_ = index;
	if (impl_->removedIndex_ == -1 || index >= (int) impl_->filterTerms_.size())
	{
		return;
	}

	int tracker = 0;
	for (auto filterItr = impl_->filterTerms_.begin(); filterItr != impl_->filterTerms_.end(); ++filterItr)
	{
		if (tracker == impl_->removedIndex_)
		{
			impl_->filterTerms_.erase( filterItr );
			break;
		}

		++tracker;
	}
}

void SimpleActiveFiltersModel::clearCurrentFilter()
{
	impl_->filterTerms_.clear();
}

void SimpleActiveFiltersModel::addFilterTerm( std::string text )
{
	impl_->addFilter( text.c_str() );
}

IListModel * SimpleActiveFiltersModel::getSavedFilters() const
{
	return &impl_->savedFilters_;
}

void SimpleActiveFiltersModel::saveNewFilter()
{
	auto preference = impl_->uiFramework_.getPreferences()->getPreference( sActiveFiltersPropertyKey );
	unsigned int propertyCount = static_cast< unsigned int >( preference->getPropertyCount() );

	// Create the new filter string
	// TODO - allow the user to input the name for the saved filter
	// JIRA - http://jira.bigworldtech.com/browse/NGT-1484
	++propertyCount;
	std::stringstream stream;
	stream << "Filter" << propertyCount;

	impl_->generateStringValue();

	bool result = preference->set( stream.str().c_str(), impl_->stringValue_ );
	if (result)
	{
		// Add this new saved filter to the list maintained by this model so it appears
		// in the QML presentation layer
		impl_->addSavedFilter( stream.str().c_str() );
	}
}

void SimpleActiveFiltersModel::loadFilter( std::string filterId )
{
	// TODO
	// JIRA: http://jira.bigworldtech.com/browse/NGT-861
}

void SimpleActiveFiltersModel::clearSavedFilters()
{
	// TODO - can't even do this as GenericObject has no way to clear its properties
	// JIRA: http://jira.bigworldtech.com/browse/NGT-861
}
