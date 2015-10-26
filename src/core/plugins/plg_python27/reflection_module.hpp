#ifndef PYTHON_REFLECTION_MODULE_HPP
#define PYTHON_REFLECTION_MODULE_HPP


class IDefinitionManager;
class IObjectManager;
namespace PythonType
{
class IConverter;
} // namespace PythonType
template < typename ITypeConverter, typename ScriptType >
class TypeConverterQueue;

namespace PyScript
{
	class ScriptObject;
} // namespace PyScript
typedef TypeConverterQueue< PythonType::IConverter,
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
