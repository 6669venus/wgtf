#pragma once
#ifndef _OBJECT_MANAGER_HPP
#define _OBJECT_MANAGER_HPP

#include "i_object_manager.hpp"

#include <unordered_map>
#include <map>
#include <memory>
#include <mutex>

struct ObjectMetaData;

namespace ReflectedPython
{


/**
 *	Python implementation of IObjectManager.
 *	
 *	The default ObjectManager will hold strong references to objects.
 *	Python object's lifetimes are controlled by ref-counting or when Python
 *	is finalized.
 *	This implementation of IObjectManager holds weak references to Python objects.
 */
class ObjectManager
	: public Implements< IPythonObjectManager >
{
public:

	ObjectManager();
	virtual ~ObjectManager();

	virtual ObjectHandle registerUnmanagedObject(
		const IClassDefinition & key,
		const ObjectHandle & object,
		const RefObjectId & id = RefObjectId::zero() ) override;
	virtual bool deregisterUnmanagedObject( const IClassDefinition & key ) override;
	virtual ObjectHandle getUnmanagedObject( const IClassDefinition & key ) const override;

	void clear();

private:
	// all references to a reflected object
	// maps id to reflected object & its references
	// Store weak-refs so that ObjectManager does not store all objects forever
	std::unordered_map< const RefObjectId,
		std::weak_ptr< ObjectMetaData > > idMap_;
	std::unordered_map< const IClassDefinition *,
		std::weak_ptr< ObjectMetaData > > unmanagedMetaDataMap_;
	mutable std::mutex objectsLock_;
};


} // namespace ReflectedPython

#endif // _OBJECT_MANAGER_HPP
