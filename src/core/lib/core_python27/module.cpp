#include "pch.hpp"

#include "module.hpp"
#include "wg_pyscript/py_script_object.hpp"


class Python27Module::Implementation
{
public:
	Implementation( PyScript::ScriptModule& module )
		: module_( module )
	{
	}

	PyScript::ScriptModule module_;
};


Python27Module::Python27Module( PyScript::ScriptModule& module )
	: impl_( new Implementation( module ) )
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