#ifndef FILTERED_LIST_MODEL_HPP
#define FILTERED_LIST_MODEL_HPP

#include "i_list_model.hpp"

#include <functional>
#include <memory>

class FilteredListModel : public IListModel
{
public:
	typedef std::function< bool ( const IItem* ) > ListFilter;

public:
	FilteredListModel( IListModel & model, const ListFilter & function );
	FilteredListModel( const FilteredListModel & rhs );
	virtual ~FilteredListModel();

	FilteredListModel & operator=( const FilteredListModel & rhs );

	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem * item ) const override;
	virtual bool empty() const override;
	virtual size_t size() const override;

	IListModel * getSource();
	const IListModel * getSource() const;

	void refresh();

private:
	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};

#endif // FILTERED_LIST_MODEL_HPP
