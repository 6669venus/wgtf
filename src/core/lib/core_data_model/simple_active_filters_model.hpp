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

	virtual const char* getStringValue() const override;
	
	virtual const int & removeFilter() const override;
	virtual void removeFilter( const int & index ) override;

	virtual const int & selectedFilter() const override;
	virtual void selectedFilter( const int & index ) override;

	virtual bool clearFilters() const override;

	// Functions that will undoubtedly change once reflected methods
	// are supported in NGT.
	virtual const std::string & addFilter() const override;
	virtual void addFilter( const std::string & text ) override;

	virtual const std::string & saveFilters() const override;
	virtual void saveFilters( const std::string & filename ) override;

	virtual const std::string & loadFilters() const override;
	virtual void loadFilters( const std::string & filename ) override;

private:

	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif //SIMPLE_ACTIVE_FILTERS_MODEL_HPP