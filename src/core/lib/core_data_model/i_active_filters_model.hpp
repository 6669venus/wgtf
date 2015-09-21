#ifndef I_ACTIVE_FILTERS_MODEL_HPP
#define I_ACTIVE_FILTERS_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include <string>

//------------------------------------------------------------------------------
// IActiveFiltersModel
//
// Represents the data model for the WGActiveFilters control.
//------------------------------------------------------------------------------

class IActiveFiltersModel
{
	DECLARE_REFLECTED

public:

	//-------------------------------------
	// Lifecycle
	//-------------------------------------

	IActiveFiltersModel() : tempObjHandle_(ObjectHandle()), tempInt_(0)
	{
		// Just a temporary implementation until type definition registration
		// allows abstract classes.
	}

	virtual ~IActiveFiltersModel() {}
	
	//-------------------------------------
	// Data Model Accessors
	//-------------------------------------
	
	virtual ObjectHandle getFilters() const { return ObjectHandle(); }

	virtual ObjectHandle getSavedFilters() const { return ObjectHandle(); }
		
	virtual void removeFilter( int index ) {}

	virtual void selectedFilter( int index ) {}

	virtual void clearFilters() {}
	
	virtual void addFilter( std::string text ) {}

	virtual void saveFilters( std::string filename ) {}

	virtual void loadFilters( std::string filename ) {}
			
public:

	// These are temporary variables. Type/definition registration does not 
	// allow for the registration of abstract classes. We need temporary
	// return values for the default implementation.
	ObjectHandle tempObjHandle_;
	std::string tempString_;
	int tempInt_;
};

#endif //I_ACTIVE_FILTERS_MODEL_HPP