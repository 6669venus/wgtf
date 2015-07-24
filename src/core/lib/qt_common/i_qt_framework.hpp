#ifndef I_QT_FRAMEWORK_HPP
#define I_QT_FRAMEWORK_HPP

#include "dependency_system/i_interface.hpp"
#include "ui_framework/i_ui_framework.hpp"

class IQtTypeConverter;
class QQmlComponent;
class QQmlEngine;
class QtPalette;
class QVariant;
class QWidget;
class Variant;

class IQtFramework : public Implements< IUIFramework >
{
public:
	virtual ~IQtFramework() {}

	virtual QQmlEngine * qmlEngine() const = 0;
	virtual const QtPalette * palette() const = 0;

	virtual void registerTypeConverter( IQtTypeConverter & converter ) = 0;
	virtual QVariant toQVariant( const Variant & variant ) const = 0;
	virtual Variant toVariant( const QVariant & qVariant ) const = 0;
	virtual void setCopyControlsEnabled( bool enabled ) {}
	virtual bool getCopyControlsEnabled() const { return false; }

	// TODO: Move into a ui adaption system
	virtual QQmlComponent * toQmlComponent( IComponent & component ) = 0;
	virtual QWidget * toQWidget( IView & view ) = 0;
};

#endif//I_QT_FRAMEWORK