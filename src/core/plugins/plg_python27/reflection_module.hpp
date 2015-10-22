#ifndef PYTHON_REFLECTION_MODULE_HPP
#define PYTHON_REFLECTION_MODULE_HPP


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
{
public:
	/**
	 *	Register this module with Python.
	 */
	ReflectionModule( IDefinitionManager & definitionManager,
		IObjectManager & objectManager,
		const PythonTypeConverters & typeConverters );
};


#endif // PYTHON_REFLECTION_MODULE_HPP
