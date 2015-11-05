#pragma once
#ifndef PYTHON_MODULE_HPP
#define PYTHON_MODULE_HPP

#include "interfaces/core_python_script/i_module.hpp"

#include <memory>


namespace PyScript
{
	class ScriptModule;
	class ScriptObject;
}


namespace ReflectedPython
{
	class DefinedInstance;
}


class IComponentContext;


/**
 *	Interface to Python 2.7.x.
 */
class Python27Module
	: public IPythonModule
{
public:
	Python27Module( IComponentContext& context, PyScript::ScriptModule& module );
	Python27Module( Python27Module&& other ); 
	Python27Module& operator=( Python27Module&& other ); 
	virtual ~Python27Module();

	bool exists() const override;
	bool callMethod( const char * methodName ) const override;
	std::unique_ptr<ReflectedPython::DefinedInstance> getObject( const char * name ) const override;

private:
	Python27Module( const Python27Module& other ); 
	Python27Module& operator=( const Python27Module& other ); 

	class Implementation;
	std::unique_ptr< Implementation > impl_;
};


#endif // PYTHON_MODULE_HPP

