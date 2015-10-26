#pragma once
#ifndef PYTHON_DEFINED_INSTANCE_HPP
#define PYTHON_DEFINED_INSTANCE_HPP


#include "core_reflection/generic/base_generic_object.hpp"
#include "wg_pyscript/py_script_object.hpp"


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
	 *	Do not use this function.
	 *	It is required to be implemented by the .mpp implementation.
	 *	But we do not register these objects with ObjectManager, so
	 *	always create this object with the other constructor provided.
	 */
	DefinedInstance();

	/**
	 *	Construct a class definition from the given Python object.
	 */
	DefinedInstance( IDefinitionManager & definitionManager,
		PyScript::ScriptObject & pythonObject,
		const PythonTypeConverters & typeConverters );
	~DefinedInstance();


	/**
	 *	Get the per-instance definition of the Python object.
	 *	@return class definition based on the given Python object.
	 */
	const IClassDefinition & getDefinition() const override;


private:
	IBaseProperty * addProperty( const char * name,
		const TypeId & typeId,
		const MetaBase * pMetaBase ) override;

	ObjectHandle getDerivedType() const override;
	ObjectHandle getDerivedType() override;

	/**
	 *	PyScript::ScriptObject wraps PyObject* and handles ref-counting and
	 *	the Python C-API.
	 */
	PyScript::ScriptObject pythonObject_;

	/**
	 *	Methods and members in pythonObject_ are added to this definition to
	 *	be used by NGT reflection.
	 */
	IClassDefinition* pDefinition_;
};



} // namespace ReflectedPython

#endif // PYTHON_DEFINED_INSTANCE_HPP
