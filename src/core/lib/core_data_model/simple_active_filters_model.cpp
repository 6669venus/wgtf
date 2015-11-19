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

struct SimpleActiveFiltersModel::Impl
{
	Impl( SimpleActiveFiltersModel& self, IDefinitionManager & definitionManager, 
		IUIFramework & uiFramework );

	SimpleActiveFiltersModel& self_;
	IDefinitionManager& definitionManager_;
	IUIFramework& uiFramework_;
	VariantList filterTerms_;
	VariantList savedFilters_;
	std::string stringValue_;
	std::string loadedFilterId_;
	int removedIndex_;

	void addFilter( const char* text );
	void addSavedFilter( const char* filterId );
	bool updateSavedFilter( const char* filterId);
	void generateStringValue();
};

SimpleActiveFiltersModel::Impl::Impl( 
	SimpleActiveFiltersModel& self,
	IDefinitionManager & definitionManager,
	IUIFramework & uiFramework)
: self_( self )
, definitionManager_( definitionManager )
, uiFramework_( uiFramework )
, stringValue_( "" )
, loadedFilterId_( "" )
, removedIndex_( -1 )
{
}

void SimpleActiveFiltersModel::Impl::addFilter( const char* text )
{
	auto filterTerm = definitionManager_.create< ActiveFilterTerm >();
	filterTerm->setValue( text );
	filterTerms_.push_back( filterTerm );
}

void SimpleActiveFiltersModel::Impl::addSavedFilter( const char* filterId )
{
	generateStringValue();

	auto savedFilter = definitionManager_.create< SavedActiveFilter >();
	
	savedFilter->setFilterId( filterId );
	savedFilter->setTerms( stringValue_ );

	savedFilters_.push_back( savedFilter );
}

bool SimpleActiveFiltersModel::Impl::updateSavedFilter( const char* filterId )
{
	generateStringValue();

	VariantList::Iterator itrFilter = savedFilters_.begin();
	VariantList::Iterator itrEnd = savedFilters_.end();
	for (; itrFilter != itrEnd; ++itrFilter)
	{
		Variant varFilter = ( *itrFilter );
		ObjectHandle provider;
		if (varFilter.tryCast< ObjectHandle >( provider ))
		{
			auto savedFilter = provider.getBase< SavedActiveFilter >();
			if (!savedFilter)
			{
				continue;
			}

			if (strcmp( savedFilter->getFilterId().c_str(), filterId ) == 0)
			{
				savedFilter->setTerms( stringValue_ );
				return true;
			}
		}
	}

	return false;
}

void SimpleActiveFiltersModel::Impl::generateStringValue()
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
, impl_( new Impl( *this, definitionManager, uiFramework ) )
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

std::string SimpleActiveFiltersModel::saveFilter( bool overwrite )
{
	auto preference = impl_->uiFramework_.getPreferences()->getPreference( sActiveFiltersPropertyKey );

	std::string savedFilterName = impl_->loadedFilterId_.c_str();
	if (!overwrite)
	{
		unsigned int propertyCount = static_cast< unsigned int >( preference->getPropertyCount() );

		// Create the new filter string
		// TODO - allow the user to input the name for the saved filter
		// JIRA - http://jira.bigworldtech.com/browse/NGT-1484
		++propertyCount;
		std::stringstream stream;
		stream << "Filter" << propertyCount;
		savedFilterName = stream.str().c_str();
	}

	impl_->generateStringValue();

	bool result = preference->set( savedFilterName.c_str(), impl_->stringValue_ );
	if (result)
	{
		// Add this new saved filter to the list maintained by this model so it appears
		// in the QML presentation layer
		if (!overwrite)
		{
			impl_->addSavedFilter( savedFilterName.c_str() );
		}
		else
		{
			// Get the loaded filter by its ID and update its terms
			if (!impl_->updateSavedFilter( savedFilterName.c_str() ))
			{
				return std::string( "" );
			}
		}

		impl_->loadedFilterId_ = savedFilterName.c_str();
		return savedFilterName;
	}

	return std::string("");
}

bool SimpleActiveFiltersModel::loadFilter( std::string filterId )
{
	// Clear the current filter terms, so we start with a clean slate
	clearCurrentFilter();

	// Fetch the saved filter property by the provided identifier
	std::string terms;
	auto preference = impl_->uiFramework_.getPreferences()->getPreference( sActiveFiltersPropertyKey );
	auto accessor = preference->findProperty( filterId.c_str() );
	if (!accessor.isValid())
	{
		return false;
	}
	
	preference->get( filterId.c_str(), terms );
	
	// Tokenize the terms and add them to the current active filter
	std::istringstream stream( terms );
	std::string token;
	while (std::getline( stream, token, ',' ))
	{
		if (token.length() > 0)
		{
			impl_->addFilter( token.c_str() );
		}
	}

	impl_->loadedFilterId_ = filterId;
	return true;
}

void SimpleActiveFiltersModel::clearSavedFilters()
{
	// TODO - can't even do this as GenericObject has no way to clear its properties
	// JIRA: http://jira.bigworldtech.com/browse/NGT-861
}
