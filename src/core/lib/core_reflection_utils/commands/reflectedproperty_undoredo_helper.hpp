#ifndef REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP
#define REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP

#include "core_variant/variant.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include <vector>
#include <memory>

class IDataStream;
class IObjectManager;
class PropertyAccessor;
class ObjectHandle;
class ISerializationManager;
class IDefinitionManager;

namespace ReflectedPropertyUndoRedoUtility
{
	struct ReflectedClassMemberUndoRedoHelper
	{
		virtual ~ReflectedClassMemberUndoRedoHelper() { }

		RefObjectId objectId_;
		std::string path_;

		virtual bool isMethod() const = 0;
	};

	struct ReflectedPropertyUndoRedoHelper: public ReflectedClassMemberUndoRedoHelper
	{
		virtual ~ReflectedPropertyUndoRedoHelper() { }

		std::string typeName_;
		Variant preValue_;
		Variant postValue_;

		bool isMethod() const override { return false; }
	};

	struct ReflectedMethodUndoRedoHelper: public ReflectedClassMemberUndoRedoHelper
	{
		virtual ~ReflectedMethodUndoRedoHelper() { }

		ReflectedMethodParameters parameters_;

		bool isMethod() const override { return true; }
	};

	typedef std::vector<std::unique_ptr<ReflectedClassMemberUndoRedoHelper>> UndoRedoHelperList;

	const char * getUndoStreamHeaderTag();
	const char * getRedoStreamHeaderTag();
	const char * getPropertyHeaderTag();
	const char * getMethodHeaderTag();

	bool performReflectedUndo( IDataStream& data,
		IObjectManager & objectManager,
		IDefinitionManager & definitionManager );
	bool performReflectedRedo( IDataStream& data,
		IObjectManager & objectManager,
		IDefinitionManager & definitionManager );

	void saveUndoData( ISerializationManager & serializationMgr, IDataStream & stream, 
		const ReflectedClassMemberUndoRedoHelper& helper );
	void saveRedoData( ISerializationManager & serializationMgr, IDataStream & stream, 
		const ReflectedClassMemberUndoRedoHelper& helper );

	void saveUndoData( ISerializationManager & serializationMgr, IDataStream & stream, 
		const ReflectedPropertyUndoRedoHelper& helper );
	void saveRedoData( ISerializationManager & serializationMgr, IDataStream & stream, 
		const ReflectedPropertyUndoRedoHelper& helper );

	void saveUndoData( ISerializationManager & serializationMgr, IDataStream & stream, 
		const ReflectedMethodUndoRedoHelper& helper );
	void saveRedoData( ISerializationManager & serializationMgr, IDataStream & stream, 
		const ReflectedMethodUndoRedoHelper& helper );

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
								  IObjectManager & objectManager,
								  IDefinitionManager & definitionManager );

	

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
		const ObjectHandle & contextObject, const char * propertyPath, IDefinitionManager & definitionManager );

	

}

#endif //REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP