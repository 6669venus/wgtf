#ifndef FILTERED_LIST_MODEL_HPP
#define FILTERED_LIST_MODEL_HPP

#include "i_list_model.hpp"
#include "core_data_model/filtering/i_item_filter.hpp"

#include <functional>
#include <memory>

class FilteredListModel : public IListModel
{
public:
	FilteredListModel();
	FilteredListModel( const FilteredListModel & rhs );
	virtual ~FilteredListModel();

	FilteredListModel & operator=( const FilteredListModel & rhs );

	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem * item ) const override;
	virtual bool empty() const override;
	virtual size_t size() const override;
	virtual int columnCount() const override;

	void setSource( IListModel * source );
	void setFilter( IItemFilter * filter );

	IListModel * getSource();
	const IListModel * getSource() const;

	void refresh( bool waitToFinish = false );

	bool isFiltering() const;

	PUBLIC_EVENT( FilteredListModel, FilteringBegin );
	PUBLIC_EVENT( FilteredListModel, FilteringEnd );

private:
	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};

#endif // FILTERED_LIST_MODEL_HPP
