#ifndef PYTHON_REFLECTION_MODULE_HPP
#define PYTHON_REFLECTION_MODULE_HPP


class IComponentContext;


class ReflectionModule
{
public:


	/**
	 *	Register this module with Python.
	 */
	ReflectionModule( IComponentContext & context );


	/**
	 *	Module cannot be de-registered with Python.
	 *	Only destroy on shutdown.
	 */
	~ReflectionModule();


	/// For use by module after it's registered
	IComponentContext & context_;
};


#endif // PYTHON_REFLECTION_MODULE_HPP
