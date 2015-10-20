#pragma once
#ifndef PYTHON_DEFINED_INSTANCE_HPP
#define PYTHON_DEFINED_INSTANCE_HPP

#include "core_reflection/utilities/reflection_utilities.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include "definition_details.hpp"

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
 *	So Defined Instance inherits from DefinitionProvider in order to provide a
 *	different class definition for each instance of PyObject*.
 *	
 *	@see GenericObject, QtScriptObject.
 */
class DefinedInstance : public DefinitionProvider
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
		PyScript::ScriptObject& pythonObject );
	~DefinedInstance();


	/**
	 *	Get the per-instance definition of the Python object.
	 *	@return class definition based on the given Python object.
	 */
	const IClassDefinition & getDefinition() const override;

	/**
	 *	Get a typed property from the Python object.
	 *	@param name name of property.
	 *	@param outValue value of property is stored here.
	 *	@return true on success.
	 */
	template< typename T >
	bool get( const char * name, T & outValue ) const;

	/**
	 *	Set a typed property on the Python object.
	 *	@param name name of property.
	 *	@param value value of property to set.
	 *	@return true on success.
	 */
	template< typename T >
	bool set( const char * name, const T & value );

	/**
	 *	Set a variant property on the Python object.
	 *	@param name name of property.
	 *	@param value value of property to set.
	 *	@return true on success.
	 */
	bool set( const char * name, Variant & value );

	/**
	 *	Call a function which is part of this instance.
	 *	
	 *	@pre the given name must be a callable function on this instance.
	 *	@pre the given parameters must be passable to the function.
	 *	
	 *	@param name the name of the function.
	 *	@param parameters the arguments to the function.
	 *	@return the return value of the function.
	 */
	Variant invoke( const char * name,
		const ReflectedMethodParameters& parameters );

private:

	Variant getProperty( const char * name ) const;
	bool setProperty( const char * name,
		const TypeId & typeId,
		Variant & value ) const;

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


template< typename T >
bool DefinedInstance::get( const char * name, T & value ) const
{
	auto pDefinitionManager = this->getDefinition().getDefinitionManager();
	assert( pDefinitionManager != nullptr );
	auto variant = this->getProperty( name );
	return ReflectionUtilities::extract( variant,
		value,
		(*pDefinitionManager) );
}


template< typename T >
bool DefinedInstance::set( const char * name, const T & value )
{
	const TypeId typeId = TypeId::getType< T >();
	auto variantValue = ReflectionUtilities::reference( value );
	return this->setProperty( name, typeId, variantValue );
}

} // namespace ReflectedPython

#endif // PYTHON_DEFINED_INSTANCE_HPP
