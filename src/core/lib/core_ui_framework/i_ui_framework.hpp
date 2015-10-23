#ifndef I_UI_FRAMEWORK_HPP
#define I_UI_FRAMEWORK_HPP

#include "core_reflection/object_handle.hpp"

#include <memory>

class IAction;
class IComponent;
class IComponentProvider;
class IView;
class IWindow;

/**
 * The UI Framework interface
 */
class IUIFramework
{
public:
    /**
     * Resource types
     */
	enum class ResourceType
	{
		Buffer, ///< Buffer resource type
		File,   ///< File resource type
		Url     ///< Url resource type
	};

	virtual ~IUIFramework() {}

	/**
     * Create an action
     *
     * @param func  Action function
     * @param enableFunc Enable function for this action
	 * @param checkedFunc checked function for this action
     * @return IAction* The created action instance
     */
	virtual std::unique_ptr< IAction > createAction(
		const char * id, std::function<void( IAction* )> func, 
		std::function<bool( IAction* )> enableFunc = [] ( IAction* ) { return true; },
		std::function<bool( IAction* )> checkedFunc = std::function<bool( IAction* )>( nullptr ) ) = 0;

    /**
     * Create component
     *
     * @param resource Resource data
     * @param type The resource type
     * @return IComponent The created component instance
     */
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
