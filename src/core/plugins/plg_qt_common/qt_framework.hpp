#ifndef QT_FRAMEWORK_HPP
#define QT_FRAMEWORK_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_action_manager.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

class IFileUtilities;
class IQtTypeConverter;
class QQmlComponent;
class QtScriptingEngine;
class IComponentContext;
class QtDefaultSpacing;
class QtGlobalSettings;
class QmlWindow;

namespace QtFramework_Locals
{
	class QtCommandEventListener;
}

class QtFramework
	: public Implements< IQtFramework >
{
public:
	QtFramework();
	virtual ~QtFramework();

	void initialise( IComponentContext & contextManager );
	void finalise();

	// IQtFramework
	QQmlEngine * qmlEngine() const override;
	const QtPalette * palette() const override;
	QtGlobalSettings * qtGlobalSettings() const override;

	void registerTypeConverter( IQtTypeConverter & converter ) override;
	QVariant toQVariant( const Variant & variant ) const override;
	Variant toVariant( const QVariant & qVariant ) const override;

	QQmlComponent * toQmlComponent( IComponent & component ) override;
	QWidget * toQWidget( IView & view ) override;

	// IUIFramework
	std::unique_ptr< IAction > createAction(
		const char * id, std::function<void()> func, 
		std::function<bool()> enableFunc ) override;
	std::unique_ptr< IComponent > createComponent( 
		const char * resource, ResourceType type ) override;
	std::unique_ptr< IView > createView( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;
	std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;

	void loadActionData( const char * resource, ResourceType type ) override;
	void registerComponent( const char * id, IComponent & component ) override;
	void registerComponentProvider( IComponentProvider & provider ) override;
	IComponent * findComponent( const TypeId & typeId, 
		std::function< bool ( size_t ) > & predicate ) const override;

	virtual void setPluginPath( const std::string& path ) override;
	virtual const std::string& getPluginPath() const override;

protected:
	virtual QmlWindow * createQmlWindow();

private:
	void registerDefaultComponents();
	void registerDefaultComponentProviders();
	void registerDefaultTypeConverters();

	std::unique_ptr< QQmlEngine > qmlEngine_;
	std::unique_ptr< QtScriptingEngine > scriptingEngine_;
	std::unique_ptr< QtPalette > palette_;
	std::unique_ptr< QtDefaultSpacing > defaultQmlSpacing_;
	std::unique_ptr< QtGlobalSettings > globalQmlSettings_;
	std::vector< std::unique_ptr< IComponent > > defaultComponents_;
	std::vector< std::unique_ptr< IComponentProvider > > defaultComponentProviders_;
	std::vector< std::unique_ptr< IQtTypeConverter > > defaultTypeConverters_;

	std::map< std::string, IComponent * > components_;
	std::vector< IComponentProvider * > componentProviders_;
	std::vector< IQtTypeConverter * > typeConverters_;

	std::string pluginPath_;

	QtActionManager actionManager_;

	std::unique_ptr< QtFramework_Locals::QtCommandEventListener > commandEventListener_;
};

#endif
