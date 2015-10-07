#ifndef PYTHON_REFLECTION_MODULE_HPP
#define PYTHON_REFLECTION_MODULE_HPP


class IDefinitionManager;
class IObjectManager;


class ReflectionModule
{
public:
	/**
	 *	Register this module with Python.
	 */
	ReflectionModule( IDefinitionManager& definitionManager,
		IObjectManager& objectManager );
};


#endif // PYTHON_REFLECTION_MODULE_HPP
