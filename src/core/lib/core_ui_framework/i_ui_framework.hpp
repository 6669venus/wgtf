#ifndef I_UI_FRAMEWORK_HPP
#define I_UI_FRAMEWORK_HPP

#include "reflection/object_handle.hpp"

#include <memory>

class IAction;
class IComponent;
class IComponentProvider;
class IView;
class IWindow;

class IUIFramework
{
public:
	enum class ResourceType
	{
		Buffer,
		File,
		Url
	};

	virtual ~IUIFramework() {}

	virtual std::unique_ptr< IAction > createAction(
		const char * id, std::function<void()> func, 
		std::function<bool()> enableFunc = [] () { return true; } ) = 0;
	virtual std::unique_ptr< IComponent > createComponent( 
		const char * resource, ResourceType type ) = 0;
	virtual std::unique_ptr< IView > createView( 
		const char * resource, ResourceType type, 
		const ObjectHandle & context = ObjectHandle() ) = 0;
	virtual std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context = ObjectHandle() ) = 0;

	virtual void loadActionData( const char * resource, ResourceType type ) = 0;
	virtual void registerComponent( const char * id, IComponent & component ) = 0;
	virtual void registerComponentProvider( IComponentProvider & provider ) = 0;
	virtual IComponent * findComponent( const TypeId & typeId, 
		std::function< bool ( size_t ) > & predicate ) const = 0;

	virtual void setPluginPath( const std::string& path ) = 0;
	virtual const std::string& getPluginPath() const = 0; 
};

#endif//I_UI_FRAMEWORK_HPP
