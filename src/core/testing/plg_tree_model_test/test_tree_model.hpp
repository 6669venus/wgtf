#ifndef TEST_TREE_MODEL_HPP
#define TEST_TREE_MODEL_HPP

#include "core_data_model/i_tree_model.hpp"

#include <memory>

class TestTreeModel: public ITreeModel
{
public:
	TestTreeModel();
	TestTreeModel( const TestTreeModel& rhs );
	virtual ~TestTreeModel();

	TestTreeModel& operator=( const TestTreeModel& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* parent ) const override;
	virtual size_t size( const IItem* parent ) const override;
	virtual int columnCount() const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override;

	void test();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif
