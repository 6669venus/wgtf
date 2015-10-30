#ifndef DEMO_OBJECTS_HPP
#define DEMO_OBJECTS_HPP

#include "core_reflection/object_handle.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "tinyxml2.hpp"
class IComponentContext;
class IDefinitionManager;
class IReflectionController;
class IListModel;
class Vector3;

class DemoObjects
{
public:
	DemoObjects();
	~DemoObjects();
	bool init( IComponentContext & contextManager );
	ObjectHandle getTreeModel() const;
	void updateRootObject( const ObjectHandle & root );
	size_t getObjectCount();
	Vector3 getObjectPosition( int index );


private:
	bool loadDemoData( IDefinitionManager & contextManager );
	void populateDemoObject( GenericObjectPtr & genericObject, const tinyxml2::XMLNode& objectNode );

	IDefinitionManager* pDefManager_;
	IReflectionController* controller_;
	GenericListT<GenericObjectPtr> objList_;
	ObjectHandle treeRootObject_;

};

#endif //DEMO_OBJECTS_HPP
