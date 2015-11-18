#pragma once
#ifndef PYTHON_META_TYPE_HPP
#define PYTHON_META_TYPE_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_python_script/i_python_meta_type.hpp"
#include "wg_pyscript/py_script_object.hpp"


#include <string>


class PythonMetaType;
bool operator==( const PythonMetaType & lhs, const PythonMetaType & rhs );
bool operator!=( const PythonMetaType & lhs, const PythonMetaType & rhs );


/**
 *	This class is for wrapping Python types inside a Variant.
 */
class PythonMetaType
	: public Implements< IPythonMetaType >
{
public:
	/**
	 *	Constructs an invalid type.
	 */
	PythonMetaType();

	/**
	 *	Can only be constructed from a Python type or a Python class.
	 */
	PythonMetaType( const PyScript::ScriptType & type );
	PythonMetaType( const PyScript::ScriptClass & type );

	PythonMetaType( const PythonMetaType & other );
	PythonMetaType( PythonMetaType && other );

	const char * name() const override;
	const PyScript::ScriptObject & type() const;

	PythonMetaType & operator=( const PythonMetaType & other );
	PythonMetaType & operator=( PythonMetaType && other );

private:
	friend bool operator==( const PythonMetaType & lhs,
		const PythonMetaType & rhs );
	friend bool operator!=( const PythonMetaType & lhs,
		const PythonMetaType & rhs );
	friend bool operator<( const PythonMetaType & lhs,
		const PythonMetaType & rhs );

	void generateName();

	PyScript::ScriptObject typeOrClass_;
	std::string nameCache_;
};


#endif // PYTHON_META_TYPE_HPP
