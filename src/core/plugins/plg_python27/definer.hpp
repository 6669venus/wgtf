#pragma once
#ifndef PYTHON_DEFINER_HPP
#define PYTHON_DEFINER_HPP

#include "core_reflection/utilities/reflection_utilities.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include "definition_details.hpp"


/**
 *	A Python Definer is the bridge between an ObjectHandle and a PyObject*.
 *	
 *	When a PyObject* is converted into a Python Definer, it will have all of its
 *	members and methods accessible via the NGT Reflection System.
 *	
 *	@see GenericObject, QtScriptObject.
 */
class Definer
{
public:
	/**
	 *	Construct a class definition from the given Python object.
	 */
	Definer( IDefinitionManager & definitionManager,
		PyScript::ScriptObject& pythonObject );
	~Definer();


	/**
	 *	@return class definition based on the given Python object.
	 */
	const IClassDefinition & getDefinition() const;


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


private:

	Variant getProperty( const char * name ) const;
	bool setProperty( const char * name,
		const TypeId & typeId,
		Variant & value ) const;

	IDefinitionManager & definitionManager_;

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
bool Definer::get( const char * name, T & value ) const
{
	auto pDefinitionManager = this->getDefinition().getDefinitionManager();
	assert( pDefinitionManager != nullptr );
	auto variant = this->getProperty( name );
	return ReflectionUtilities::extract( variant,
		value,
		(*pDefinitionManager) );
}


template< typename T >
bool Definer::set( const char * name, const T & value )
{
	const TypeId typeId = TypeId::getType< T >();
	auto variantValue = ReflectionUtilities::reference( value );
	return this->setProperty( name, typeId, variantValue );
}


#endif // PYTHON_DEFINER_HPP
