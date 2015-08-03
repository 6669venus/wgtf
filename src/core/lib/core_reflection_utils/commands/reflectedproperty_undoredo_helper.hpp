#ifndef REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP
#define REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP
#include <vector>
#include "variant/variant.hpp"
#include "reflection/ref_object_id.hpp"

class IDataStream;
class IObjectManager;
class PropertyAccessor;
class ObjectHandle;
class ISerializationManager;

namespace ReflectedPropertyUndoRedoUtility
{
	struct ReflectionPropertyUndoRedoHelper
	{
		RefObjectId objectId_;		// object id
		std::string propertyPath_;	// property path
		std::string	propertyTypeName_;	// property type
		Variant		preValue_;		// pre value
		Variant     postValue_;		// post value
	};

	typedef std::vector< ReflectionPropertyUndoRedoHelper > UndoRedoHelperList;

	const char * getUndoStreamHeaderTag();
	const char * getRedoStreamHeaderTag();
	const char * getPropertyHeaderTag();

	bool performReflectedUndo( IDataStream& data,
		IObjectManager & objectManager );
	bool performReflectedRedo( IDataStream& data,
		IObjectManager & objectManager );
	void saveUndoData( ISerializationManager & serializationMgr, 
		IDataStream & stream, 
		const ReflectionPropertyUndoRedoHelper& helper );
	void saveRedoData( ISerializationManager & serializationMgr, 
		IDataStream & stream, 
		const ReflectionPropertyUndoRedoHelper& helper );

	/**
	*	Reads reflected properties from a data stream into a cache.
	*	@param outPropertyCache cache to fill.
	*	@param undoStream data stream from which to read.
	*	@param redoStream data stream from which to read.
	*	@return success.
	*/
	bool loadReflectedProperties( UndoRedoHelperList & outPropertyCache,
								  IDataStream & undoStream,
								  IDataStream & redoStream,
								  IObjectManager & objectManager );

	

	/**
	 *	Resolve the property path for context object by a given property path
	 *  resolve strategy:
	 *  suppose the property path stored in macro is a.b.c.d.e
	 *  if context object has a property which path is one of 
	 *  the following value, we could bind it
	 *  $X.a.b.c.d.e or $X.b.c.d.e or $X.c.d.e or $X.d.e or$X.e
	 *  $X could be any of property path including empty string
	 *	@param contextObject: a context object which try to bind property with.
	 *	@param propertyPath: the property path which context object try to bind to.
	 *	@return a new property path that context object can bind to.
	 */
	std::string resolveContextObjectPropertyPath( 
		const ObjectHandle & contextObject, const char * propertyPath );

	

}

#endif //REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP