#ifndef QT_FRAMEWORK_HPP
#define QT_FRAMEWORK_HPP

#include "dependency_system/i_interface.hpp"
#include "qt_common/i_qt_framework.hpp"
#include "ui_framework/i_ui_framework.hpp"

class IFileUtilities;
class IQtTypeConverter;
class QQmlComponent;
class QtScriptingEngine;
class IContextManager;

class QtFramework
	: public Implements< IQtFramework >
{
public:
	QtFramework();
	virtual ~QtFramework();

	void initialise( IContextManager & contextManager );
	void finalise();

	// IQtFramework
	QQmlEngine * qmlEngine() const override;
	const QtPalette * palette() const override;

	void registerTypeConverter( IQtTypeConverter & converter ) override;
	QVariant toQVariant( const Variant & variant ) const override;
	Variant toVariant( const QVariant & qVariant ) const override;

	QQmlComponent * toQmlComponent( IComponent & component ) override;
	QWidget * toQWidget( IView & view ) override;

	// IUIFramework
	std::unique_ptr< IComponent > createComponent( 
		const char * resource, ResourceType type ) override;
	std::unique_ptr< IView > createView( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;
	std::unique_ptr< IWindow > createWindow( 
		const char * resource, ResourceType type,
		const ObjectHandle & context ) override;

	void registerComponent( const char * id, IComponent & component ) override;
	void registerComponentProvider( IComponentProvider & provider ) override;
	IComponent * findComponent( const TypeId & typeId, 
		std::function< bool ( size_t ) > & predicate ) const override;

private:
	void registerDefaultComponents( IFileUtilities & fileUtilities );
	void registerDefaultComponentProviders();
	void registerDefaultTypeConverters();

	std::unique_ptr< QQmlEngine > qmlEngine_;
	std::unique_ptr< QtScriptingEngine > scriptingEngine_;
	std::unique_ptr< QtPalette > palette_;
	std::vector< std::unique_ptr< IComponent > > defaultComponents_;
	std::vector< std::unique_ptr< IComponentProvider > > defaultComponentProviders_;
	std::vector< std::unique_ptr< IQtTypeConverter > > defaultTypeConverters_;

	std::map< std::string, IComponent * > components_;
	std::vector< IComponentProvider * > componentProviders_;
	std::vector< IQtTypeConverter * > typeConverters_;
};

#endif
