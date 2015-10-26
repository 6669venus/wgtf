#ifndef PYTHON_REFLECTION_MODULE_HPP
#define PYTHON_REFLECTION_MODULE_HPP


#include "core_dependency_system/depends.hpp"
#include "wg_pyscript/py_script_object.hpp"


class IDefinitionManager;
class IObjectManager;
class IPythonTypeConverter;
template < typename ITypeConverter, typename ScriptType >
class TypeConverterQueue;

namespace PyScript
{
	class ScriptObject;
} // namespace PyScript
typedef TypeConverterQueue< IPythonTypeConverter,
	PyScript::ScriptObject > PythonTypeConverters;


class ReflectionModule
	: public Depends< IDefinitionManager, IObjectManager >
{
public:
	typedef Depends< IDefinitionManager,
		IObjectManager > DepsBase;
	/**
	 *	Register this module with Python.
	 */
	ReflectionModule( IComponentContext & context,
		const PythonTypeConverters & typeConverters );
	~ReflectionModule();

	const PythonTypeConverters & typeConverters_;
};


#endif // PYTHON_REFLECTION_MODULE_HPP
