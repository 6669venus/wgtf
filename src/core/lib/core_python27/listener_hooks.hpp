#pragma once

#include "wg_pyscript/py_script_object.hpp"


class IComponentContext;
extern IComponentContext * g_pHookContext;


/**
 *	Hook for listening to property changes on new-style classes.
 *
 *	Python functions should stay in the global namespace.
 *
 *	@param self Python object.
 *	@param name of attribute to be set.
 *		May add a new attribute if one does not exist.
 *	@param value to be set on the attribute.
 *	@return -1 on error.
 */
int pySetattrHook( PyObject * self, PyObject * name, PyObject * value );


/**
 *	Hook for listening to property changes on old-style classes.
 *	@see instance_setattr(PyInstanceObject *inst, PyObject *name, PyObject *v)
 *
 *	Python functions should stay in the global namespace.
 *
 *	@param inst Python object.
 *	@param name of attribute to be set.
 *		May add a new attribute if one does not exist.
 *	@param value to be set on the attribute.
 *	@return -1 on error.
 */
int pyInstanceSetattrHook( PyInstanceObject * inst,
	PyObject * name,
	PyObject * v );


namespace ReflectedPython
{


/**
 *	Key compare functor.
 *	Need to do a deep compare on PyScript::ScriptObject to prevent getting
 *	copies of the same object added to the map.
 */
class ScriptObjectCompare
{
public:
	bool operator()( const PyScript::ScriptObject & a,
		const PyScript::ScriptObject & b ) const
	{
		return a.compareTo( b, PyScript::ScriptErrorPrint() ) < 0;
	}
};


/**
 *	Structure for tracking the number of reflected Python objects that are using
 *	a given Python type.
 */
struct HookInfo
{
	size_t hookCount;
	setattrofunc oldHook;
};
typedef std::map< PyScript::ScriptType, HookInfo, ScriptObjectCompare > HookLookup;


/**
 *	Attach hooks for listening to setattr and delattr.
 *	So that the GUI can be notified when Python objects change.
 *	@param pythonObject attach hooks to this type of object.
 *		Note: that it will attach hooks to the *type* and not the *instance*.
 *	@param hookLookup map to keep track of how many instances are hooked for
 *		a given type.
 */
void attachListenerHooks( PyScript::ScriptObject & pythonObject,
	HookLookup & hookLookup );


/**
 *	@see attachListenerHooks()
 */
void detachListenerHooks( PyScript::ScriptObject & pythonObject,
	HookLookup & hookLookup );


/**
 *	Detach all listener hooks.
 *	@param hookLookup hooks to detach.
 */
void cleanupListenerHooks( HookLookup & hookLookup );

} // namespace ReflectedPython

