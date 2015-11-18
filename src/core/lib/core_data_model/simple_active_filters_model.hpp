#ifndef SIMPLE_ACTIVE_FILTERS_MODEL_HPP
#define SIMPLE_ACTIVE_FILTERS_MODEL_HPP

#include "i_active_filters_model.hpp"

class IDefinitionManager;
class IUIFramework;

//------------------------------------------------------------------------------
// SimpleActiveFiltersModel
//
// A very simple implementation of an active filters model that may be used
// with the active filters control (WGActiveFilters).
//------------------------------------------------------------------------------

class SimpleActiveFiltersModel : public IActiveFiltersModel
{
public:

	//
	// IActiveFiltersModel Implementation
	//

	SimpleActiveFiltersModel( IDefinitionManager & definitionManager, IUIFramework & uiFramework );

	virtual ~SimpleActiveFiltersModel() override;
	
	//
	// IActiveFiltersModel Implementation
	//

	virtual IListModel * getCurrentFilterTerms() const override;
		
	virtual void removeFilterTerm( int index ) override;

	virtual void clearCurrentFilter() override;

	virtual void addFilterTerm( std::string text ) override;

	virtual IListModel * getSavedFilters() const override;

	virtual void saveNewFilter() override;

	virtual void loadFilter( std::string filterId ) override;

	virtual void clearSavedFilters() override;

private:

	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif //SIMPLE_ACTIVE_FILTERS_MODEL_HPP