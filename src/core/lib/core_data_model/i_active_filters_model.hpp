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

	IActiveFiltersModel() : tempInt_(0), tempObjHandle_(ObjectHandle())
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

	virtual const char* getStringValue() const {	return nullptr;	}
	
	virtual const int & removeFilter() const { return tempInt_; }
	virtual void removeFilter( const int & index ) {}

	virtual const int & selectedFilter() const { return tempInt_; }
	virtual void selectedFilter( const int & index ) {}

	virtual bool clearFilters() const { return true; }
	
	//
	// TODO: Once reflected functions are supported, it should not longer be
	//       necessary to have these workarounds to make a native call with
	//       parameters. The remaining functions require such workarounds.
	//
	// Current JIRA Ticket for addressing this issue:
	//		http://jira.bigworldtech.com/browse/NGT-823
	//

	virtual const std::string & addFilter() const { return tempString_; }
	virtual void addFilter( const std::string & text ) {}

	virtual const std::string & saveFilters() const { return tempString_; }
	virtual void saveFilters( const std::string & filename ) {}

	virtual const std::string & loadFilters() const { return tempString_; }
	virtual void loadFilters( const std::string & filename ) {}
			
public:

	// These are temporary variables. Type/definition registration does not 
	// allow for the registration of abstract classes. We need temporary
	// return values for the default implementation.
	ObjectHandle tempObjHandle_;
	std::string tempString_;
	int tempInt_;
};

#endif //I_ACTIVE_FILTERS_MODEL_HPP