#ifndef ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP
#define ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/i_item.hpp"
#include <memory>

class IDefinitionManager;

//------------------------------------------------------------------------------

class ActiveFiltersTestViewModel
{
	DECLARE_REFLECTED
	
public:
	ActiveFiltersTestViewModel();
	~ActiveFiltersTestViewModel();
	
	void init( IDefinitionManager & defManager );

	ObjectHandle getSimpleActiveFiltersModel() const;
	ObjectHandle getSampleDataToFilter() const ;

private:
	struct Implementation;
	Implementation* impl_;
};

//------------------------------------------------------------------------------

class SampleActiveFiltersTreeItem: public IItem
{
public:
	SampleActiveFiltersTreeItem( const char* name, const IItem* parent );
	SampleActiveFiltersTreeItem( const SampleActiveFiltersTreeItem& rhs );
	virtual ~SampleActiveFiltersTreeItem();

	SampleActiveFiltersTreeItem& operator=( const SampleActiveFiltersTreeItem& rhs );

	const IItem* getParent() const;
	virtual int columnCount() const;
	virtual const char* getDisplayText( int column ) const;
	virtual ThumbnailData getThumbnail( int column ) const;
	virtual Variant getData( int column, size_t roleId ) const;
	virtual bool setData( int column, size_t roleId, const Variant& data );

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

//------------------------------------------------------------------------------

class SampleActiveFiltersTreeModel: public ITreeModel
{
public:
	SampleActiveFiltersTreeModel();
	SampleActiveFiltersTreeModel( const SampleActiveFiltersTreeModel& rhs );
	virtual ~SampleActiveFiltersTreeModel();

	SampleActiveFiltersTreeModel& operator=( const SampleActiveFiltersTreeModel& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* parent ) const override;
	virtual size_t size( const IItem* parent ) const override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // ACTIVE_FILTERS_TEST_VIEW_MODEL_HPP