#ifndef BUGGED_LIST_VIEW_MODEL_HPP
#define BUGGED_LIST_VIEW_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/generic_list.hpp"
#include <memory>

class IDefinitionManager;

//------------------------------------------------------------------------------

class BuggedListViewModel
{	
public:
	BuggedListViewModel();
	~BuggedListViewModel();
	
	void init( IDefinitionManager & defManager );

	ObjectHandle getSimpleActiveFiltersModel() const;
	ObjectHandle getSampleDataToFilter() const;
	bool generateNewList() const;

private:
	struct Implementation;
	Implementation* impl_;
};

class ExampleListItem : public GenericListItem
{
public:
	ExampleListItem( const Variant& value );
	ExampleListItem( Variant&& value );
		
	virtual ~ExampleListItem() {}

	void setName( const char* value ) { name_ = value; }

	virtual int columnCount() const override { return 1; }

	virtual const char * getDisplayText( int column ) const override;

	virtual ThumbnailData getThumbnail( int column ) const { return nullptr; }
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override { return false; }

	const char* getName() const { return name_.c_str(); }

private:

	std::string name_;
};

#endif // BUGGED_LIST_VIEW_MODEL_HPP