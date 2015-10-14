#ifndef SIMPLE_ACTIVE_FILTERS_MODEL_HPP
#define SIMPLE_ACTIVE_FILTERS_MODEL_HPP

#include "i_active_filters_model.hpp"

class IDefinitionManager;

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

	SimpleActiveFiltersModel( IDefinitionManager & definitionManager );

	virtual ~SimpleActiveFiltersModel() override;
	
	//
	// IActiveFiltersModel Implementation
	//

	virtual IListModel * getFilters() const override;

	virtual ObjectHandle getSavedFilters() const override;
		
	virtual void removeFilter( int index ) override;

	virtual void selectedFilter( int index ) override;

	virtual void clearFilters() override;

	// Functions that will undoubtedly change once reflected methods
	// are supported in NGT.
	virtual void addFilter( std::string text ) override;

	virtual void saveFilters( std::string filename ) override;

	virtual void loadFilters( std::string filename ) override;

private:

	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif //SIMPLE_ACTIVE_FILTERS_MODEL_HPP