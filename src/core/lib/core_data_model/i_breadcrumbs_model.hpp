#ifndef I_BREADCRUMBS_MODEL_HPP
#define I_BREADCRUMBS_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "i_tree_model.hpp"
#include "variant_list.hpp"
#include <string>

class IListModel;

//------------------------------------------------------------------------------
// BaseBreadcrumbItem
//
// Represents a single breadcrumb item in its most basic form. May be
// overridden for customized functionality within your IBreadcrumbsModel.
//------------------------------------------------------------------------------

class BaseBreadcrumbItem
{
	DECLARE_REFLECTED

public:

	BaseBreadcrumbItem() : fullPath_( "" ), displayValue_( "" ) {}

	virtual ~BaseBreadcrumbItem() {}

	virtual void initialise( const IItem & item, const char * fullPath, const char * displayValue ) 
	{
		item_ = &item;
		fullPath_ = fullPath;
		displayValue_ = displayValue;
	}

	virtual void addSubItem( const Variant & breadcrumb )
	{
		subItems_.push_back( breadcrumb );
	}

	virtual const Variant* getSubItem( unsigned int index ) const
	{
		if (index < subItems_.size())
		{
			return &subItems_[index];
		}

		return nullptr;
	}

	virtual const std::string & getDisplayValue() const { return displayValue_; }
	virtual const std::string & getFullPath() const { return fullPath_; }
	virtual const IItem * getItem() const { return item_; }
	virtual const IListModel * getSubItems() const { return &subItems_; }

protected:
	
	std::string fullPath_;
	std::string displayValue_;
	VariantList subItems_;
	const IItem * item_;
};

//------------------------------------------------------------------------------
// IBreadcrumbsModel
//
// Represents the data model for the breadcrumbs control.
//------------------------------------------------------------------------------

class IBreadcrumbsModel
{
	DECLARE_REFLECTED

public:

	//-------------------------------------
	// Lifecycle
	//-------------------------------------

	IBreadcrumbsModel()
	{
		// Just a temporary implementation until type definition registration
		// allows abstract classes.
	}

	virtual ~IBreadcrumbsModel() {}
	
	//-------------------------------------
	// Data Model Accessors
	//-------------------------------------
	
	// Returns the breadcrumb items
	// Expected: IListModel of IBreadcrumbItem objects
	virtual IListModel * getBreadcrumbs() const { return nullptr; }

	// Returns the full path of the breadcrumbs in a format that may be presented as a string
	virtual const char * getPath() const { return nullptr; }

	// Returns the IItem tied to the breadcrumb at the specified index
	virtual Variant getItemAtIndex( unsigned int index, unsigned int childIndex ) { return Variant(); }

	// Clears the current set of breadcrumbs
	virtual void clear() {}

	// Returns the number of top-level breadcrumbs stored in the model
	virtual size_t size() const { return 0; }
};

#endif //I_BREADCRUMBS_MODEL_HPP