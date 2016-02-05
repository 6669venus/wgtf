#pragma once
#ifndef _PYTHON_LISTENER_HOOKS_HPP
#define _PYTHON_LISTENER_HOOKS_HPP


class IComponentContext;
extern IComponentContext * g_pHookContext;


/**
 *	Hook for listening to property changes on new-style classes.
 */
int py_setattr_hook( PyObject * self, PyObject * name, PyObject * value );


/**
 *	Hook for listening to property changes on old-style classes.
 *	@see instance_setattr(PyInstanceObject *inst, PyObject *name, PyObject *v)
 */
int py_instance_setattr_hook( PyInstanceObject * inst,
	PyObject * name,
	PyObject * v );


#endif // _PYTHON_LISTENER_HOOKS_HPP

