#ifndef CONTROL_MANAGER_HPP
#define CONTROL_MANAGER_HPP

#include "reflection/i_object_manager.hpp"

#include <memory>

class IDefinitionManager;
class IReflectionController;
class IUIApplication;
class IUIFramework;
class IView;

class ControlManager
	: public Implements< IObjectManagerListener >
{
public:
	ControlManager();
	virtual ~ControlManager();

	virtual void onObjectRegistered( const ObjectHandle & object );
	virtual void onObjectDeregistered( const ObjectHandle & object );

	void initialise();
	void finalise();

private:
	void createWidget( const ObjectHandle& object );

	typedef std::map< ObjectHandle, std::unique_ptr< IView > >
		ObjectsCollection;
	ObjectsCollection objects_;

	IUIApplication * uiApplication_;
	IUIFramework * uiFramework_;
	IDefinitionManager * defManager_;
	IReflectionController * controller_;
};

#endif // CONTROL_MANAGER_HPP
