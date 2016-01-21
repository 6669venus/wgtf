#ifndef QT_SCRIPT_OBJECT_HPP
#define QT_SCRIPT_OBJECT_HPP

/*
QtScriptObject is the bridge between an ObjectHandle and a QML type.
When an ObjectHandle is converted into a QtScriptObject, all
exposed properties will be accessible via QML as if it was a
QML type.
In addition, access is provided to meta attributes via
QML callable functions getMetaObject and containsMetaType.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/di_ref.hpp"

#include <QObject>

class MetaBase;
class IReflectionController;
class Variant;
class IBaseProperty;
class QtScriptingEngine;

class QtScriptObject : public QObject
{
public:
	QtScriptObject(
		IComponentContext& contextManager,
		QtScriptingEngine& scriptEngine,
		const QMetaObject & metaObject, 
		const ObjectHandle & object,
		QObject * parent = nullptr );
	virtual ~QtScriptObject();

	const ObjectHandle & object() const;
	const QMetaObject * metaObject() const override;
	int qt_metacall( QMetaObject::Call c, int id, void **argv ) override;

	void firePropertySignal( IBaseProperty* property, const Variant& value );
	void fireMethodSignal( IBaseProperty* method, bool undo = false );

private:
	QtScriptObject( const QtScriptObject & );

	void callMethod( int id, void **argv );
	MetaHandle getMetaObject(
		const IClassDefinition* definition,
		const QString& property ) const;
	MetaHandle getMetaObject(
		const IClassDefinition* definition,
		const QString& property,
		const QString& metaType ) const;

	DIRef<IDefinitionManager> definitionManager_;
	DIRef<IReflectionController> controller_;
	QtScriptingEngine& scriptEngine_;
	const QMetaObject & metaObject_;
	ObjectHandle object_;
};

#endif//QT_SCRIPT_OBJECT_HPP