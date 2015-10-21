#pragma once
#ifndef PYTHON_PROPERTY_HPP
#define PYTHON_PROPERTY_HPP


#include "core_reflection/interfaces/i_base_property.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace ReflectedPython
{


/**
 *	Python-specific property that belongs to a Definition.
 */
class Property : public IBaseProperty
{
public:
	/**
	 *	Construct a property given the attribute name and value.
	 *	
	 *	@param key name of the attribute. The property will copy the string
	 *		to its own storage.
	 *	@param attribute value of the attribute. Keeps a reference.
	 */
	Property( const char* key, PyScript::ScriptObject& pythonObject );

	const TypeId & getType() const override;

	const char * getName() const override;

	const MetaBase * getMetaData() const override;

	bool readOnly() const override;

	bool isMethod() const override;

	bool set( const ObjectHandle & handle,
		const Variant & value,
		const IDefinitionManager & definitionManager ) const override;

	Variant get( const ObjectHandle & handle,
		const IDefinitionManager & definitionManager ) const override;

	Variant invoke( const ObjectHandle& object,
		const ReflectedMethodParameters& parameters ) override;

	size_t parameterCount() const override;

private:
	// Need to store a copy of the string
	std::string key_;
	PyScript::ScriptObject pythonObject_;
};


} // namespace ReflectedPython

#endif // PYTHON_PROPERTY_HPP
