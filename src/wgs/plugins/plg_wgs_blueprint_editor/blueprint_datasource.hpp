#ifndef BLUEPRINT_DATASOURCE_HPP
#define BLUEPRINT_DATASOURCE_HPP 

#include "interfaces/i_datasource.hpp"
#include "dependency_system/i_interface.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/i_object_manager.hpp"

#include <unordered_map>

class BlueprintDataSource
	: public Implements<IDataSource>
	, public IObjectManagerListener
{
public:
	BlueprintDataSource();
	virtual ~BlueprintDataSource();

	void init(IComponentContext &contextManager);
	void fini(IComponentContext &contextManager);
	const ObjectHandleT<BlueprintPage> &getBlueprintPage() const;

private:

	// IObjectManagerListener
	void onObjectRegistered(const ObjectHandle &pObj);
	void onObjectDeregistered(const ObjectHandle &pObj);

	std::string blueprintPageId_;
	ObjectHandleT<BlueprintPage> blueprintPage_;
	std::unordered_map<std::string, ObjectHandle> loadedObj_;
};


#endif // BLUEPRINT_DATASOURCE_HPP