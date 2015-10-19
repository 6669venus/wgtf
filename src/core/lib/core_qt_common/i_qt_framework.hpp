#ifndef I_QT_FRAMEWORK_HPP
#define I_QT_FRAMEWORK_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

class IQtTypeConverter;
class QQmlComponent;
class QQmlEngine;
class QtPalette;
class QtGlobalSettings;
class QVariant;
class QWidget;
class Variant;

class IQtFramework : public Implements< IUIFramework >
{
public:
	virtual ~IQtFramework() {}

	virtual QQmlEngine * qmlEngine() const = 0;
	virtual const QtPalette * palette() const = 0;
	virtual QtGlobalSettings * qtGlobalSettings() const = 0;

	virtual void registerTypeConverter( IQtTypeConverter & converter ) = 0;
	virtual bool registerResourceData( const unsigned char * qrc_struct, const unsigned char * qrc_name,
		const unsigned char * qrc_data ) = 0;
	virtual QVariant toQVariant( const Variant & variant ) const = 0;
	virtual Variant toVariant( const QVariant & qVariant ) const = 0;

	// TODO: Move into a ui adaption system
	virtual QQmlComponent * toQmlComponent( IComponent & component ) = 0;
	virtual QWidget * toQWidget( IView & view ) = 0;
	virtual void retainQWidget( IView & view ) = 0;
};

#endif//I_QT_FRAMEWORK