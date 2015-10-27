#pragma once
#ifndef PYTHON_MODULE_HPP
#define PYTHON_MODULE_HPP

#include "interfaces/core_python_script/i_module.hpp"
#include "wg_pyscript/py_script_object.hpp"

#include <memory>

/**
 *	Interface to Python 2.7.x.
 */
class Python27Module
	: public IPythonModule
{
public:
	Python27Module( PyScript::ScriptModule& module );
	Python27Module( Python27Module&& other ); 
	Python27Module& operator=( Python27Module&& other ); 
	virtual ~Python27Module();

	bool exists() const override;
	bool callMethod( const char * methodName ) const override;

private:
	Python27Module( const Python27Module& other ); 
	Python27Module& operator=( const Python27Module& other ); 

	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


#endif // PYTHON_MODULE_HPP

