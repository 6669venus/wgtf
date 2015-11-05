#include "pch.hpp"

#include "module.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"


class Python27Module::Implementation
{
public:
	Implementation( IComponentContext& context, PyScript::ScriptModule& module )
		: context_( context )
		, module_( module )
	{
	}

	IComponentContext& context_;
	PyScript::ScriptModule module_;
};


Python27Module::Python27Module( IComponentContext& context, PyScript::ScriptModule& module )
	: impl_( new Implementation( context, module ) )
{
}


Python27Module::Python27Module( Python27Module&& other )
	: impl_( std::move( other.impl_ ) )
{
}


Python27Module& Python27Module::operator=( Python27Module&& other )
{
	impl_ = std::move( other.impl_ );
	return (*this);
}


Python27Module::~Python27Module()
{
}


bool Python27Module::exists() const
{
	return (impl_->module_.get() != nullptr);
}


bool Python27Module::callMethod( const char * methodName ) const
{
	PyScript::ScriptErrorRetain errorHandler;

	PyScript::ScriptObject result = impl_->module_.callMethod( methodName,
		errorHandler );

	// Test passed/failed
	const bool passed = !PyScript::Script::hasError();
	if (!passed)
	{
		PyScript::Script::printError();
		PyScript::Script::clearError();
	}

	// TODO return result instead
	return passed;
}


std::unique_ptr<ReflectedPython::DefinedInstance> Python27Module::getObject( const char * name ) const
{
//	PyScript::ScriptErrorRetain errorHandler;
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptObject scriptObject = impl_->module_.getAttribute( name, errorHandler );
	ReflectedPython::DefinedInstance* instance = nullptr;

	if (scriptObject.exists())
	{
		instance = new ReflectedPython::DefinedInstance( impl_->context_, scriptObject );
	}

	return std::unique_ptr<ReflectedPython::DefinedInstance>( instance );
}
