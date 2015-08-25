#ifndef FILTERED_TREE_MODEL_HPP
#define FILTERED_TREE_MODEL_HPP

#include "i_tree_model.hpp"

#include <memory>

class FilteredTreeModel : public ITreeModel
{
public:
	typedef std::function<bool ( const IItem* )> Filter;

public:
	FilteredTreeModel( ITreeModel& model, const Filter& function );
	FilteredTreeModel( const FilteredTreeModel& rhs );
	virtual ~FilteredTreeModel();

	FilteredTreeModel& operator=( const FilteredTreeModel& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* item ) const override;
	virtual size_t size( const IItem* item ) const override;

	ITreeModel* getSource();
	const ITreeModel* getSource() const;

	void refresh( bool wait = false );

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif
