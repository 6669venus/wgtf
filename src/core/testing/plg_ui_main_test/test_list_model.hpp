#ifndef TEST_LIST_MODEL_HPP
#define TEST_LIST_MODEL_HPP

#include "core_data_model/i_list_model.hpp"

#include <memory>

class TestListModel: public IListModel
{
public:
	TestListModel( bool shortList = false );
	TestListModel( const TestListModel& rhs );
	virtual ~TestListModel();

	TestListModel& operator=( const TestListModel& rhs );

	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem * item ) const override;

	virtual bool empty() const override;
	virtual size_t size() const override;

	virtual bool canClear() const override;
	virtual void clear() override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif
