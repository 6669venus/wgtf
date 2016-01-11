#pragma once
#ifndef _I_PYTHON_OBJECT_MANAGER_HPP
#define _I_PYTHON_OBJECT_MANAGER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/ref_object_id.hpp"

class IClassDefinition;
class ObjectHandle;


class IPythonObjectManager
{
public:
	/**
	 *	Get an existing, unmanaged object by a pointer to another object.
	 *	Convert your unregistered object to one registered with IObjectManager.
	 *	@param pObj pointer to object to be found.
	 *	@return found object or null.
	 */
	virtual ObjectHandle getUnmanagedObject( const IClassDefinition & key ) const = 0;

	/**
	 *	Register an unmanaged object.
	 *	@param pObj the object to register.
	 *	@param id the ID of the object to register.
	 *		Or zero to generate a new ID.
	 *	@return the registered object.
	 */
	virtual RefObjectId registerUnmanagedObject(
		const IClassDefinition & key,
		const ObjectHandle & object, 
		const RefObjectId & id = RefObjectId::zero() ) = 0;

	/**
	 *	Deregister an unmanaged object.
	 *	@param pObj the object to deregister.
	 *	@return true if the object was found to be deregistered.
	 */
	virtual bool deregisterUnmanagedObject( const IClassDefinition & key ) = 0;
};

#endif // _I_PYTHON_OBJECT_MANAGER_HPP
