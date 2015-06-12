#ifndef TEST_DATASOURCE_HPP
#define TEST_DATASOURCE_HPP 

#include <unordered_map>
#include "interfaces/i_datasource.hpp"
#include "dependency_system/i_interface.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/i_object_manager.hpp"


class TestDataSource
	: public Implements< IDataSource >
	, public IObjectManagerListener
{
public:
	TestDataSource();
	virtual ~TestDataSource();

	void init( IContextManager & contextManager );
	void fini( IContextManager & contextManager );
	const ObjectHandleT< TestPage > & getTestPage() const;
	const ObjectHandleT< TestPage2 > & getTestPage2() const;
	std::shared_ptr< BinaryBlock > getThumbnailImage();

	void setPolyStructObj( const ReflectedPolyStructPtr&  polyStruct );
	const ReflectedPolyStructPtr & getPolyStructObj() const;

private:

	// IObjectManagerListener
	void onObjectRegistered(const ObjectHandle & pObj);
	void onObjectDeregistered(const ObjectHandle & pObj);

	std::string testPageId_;
	std::string testPageId2_;
	ObjectHandleT< TestPage > testPage_;
	ObjectHandleT< TestPage2 > testPage2_;
	std::unordered_map<std::string, ObjectHandle > loadedObj_;
};


#endif // TEST_DATASOURCE_HPP