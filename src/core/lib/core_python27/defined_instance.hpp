#pragma once
#ifndef PYTHON_DEFINED_INSTANCE_HPP
#define PYTHON_DEFINED_INSTANCE_HPP


#include "core_reflection/generic/base_generic_object.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <memory>


class IComponentContext;


namespace ReflectedPython
{


/**
 *	A Python Defined Instance is the bridge between an ObjectHandle and a
 *	PyObject*.
 *	
 *	When a PyObject* is converted into a Defined Instance, it will have all of
 *	its members and methods accessible via the NGT Reflection System.
 *	
 *	Python definitions are "generic types".
 *	
 *	Generic types are not based on a static class definition, like a C++ class.
 *	Because generic types can add and remove members at runtime, they must
 *	provide a different class definition per instance.
 *	
 *	So Defined Instance inherits from BaseGenericObject in order to provide a
 *	different class definition for each instance of PyObject*.
 *	
 *	@see GenericObject, QtScriptObject.
 */
class DefinedInstance : public BaseGenericObject
{
public:

	/**
	 *	Do not use this function. Use DefinedInstance::create().
	 *	It is required to be implemented by the .mpp implementation.
	 *	But the lifetime of Python objects cannot managed by ObjectManager, so
	 *	always create this object with the other constructor provided.
	 */
	DefinedInstance();
	~DefinedInstance();

	static ObjectHandle create( IComponentContext & context,
		const PyScript::ScriptObject & pythonObject );

	const PyScript::ScriptObject & pythonObject() const;

private:
	/**
	 *	Construct a class definition from the given Python object.
	 */
	DefinedInstance(
		IComponentContext & context,
		const PyScript::ScriptObject & pythonObject,
		std::shared_ptr< IClassDefinition > & definition );

	IBaseProperty * addProperty( const char * name,
		const TypeId & typeId,
		const MetaBase * pMetaBase ) override;

	ObjectHandle getDerivedType() const override;
	ObjectHandle getDerivedType() override;

	/**
	 *	This is here purely to keep a reference to the Python object.
	 */
	const PyScript::ScriptObject pythonObject_;

	/**
	 *	Methods and members in pythonObject_ are added to this definition to
	 *	be used by NGT reflection.
	 */
	std::shared_ptr<IClassDefinition> pDefinition_;

	IComponentContext * context_;
};



} // namespace ReflectedPython

#endif // PYTHON_DEFINED_INSTANCE_HPP
