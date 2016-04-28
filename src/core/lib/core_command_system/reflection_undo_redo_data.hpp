#ifndef REFLECTION_UNDO_REDO_DATA_HPP
#define REFLECTION_UNDO_REDO_DATA_HPP

#include "core_serialization/resizing_memory_stream.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

class CommandInstance;
class PropertyAccessorListener;

class ReflectionUndoRedoData
{
public:
	ReflectionUndoRedoData( CommandInstance & commandInstance );

	void connect();
	void disconnect();

	void undo();
	void redo();

	ObjectHandle getCommandDescription() const;

private:
	CommandInstance &			commandInstance_;
	ResizingMemoryStream		undoData_;
	ResizingMemoryStream		redoData_;
	std::shared_ptr< PropertyAccessorListener > paListener_;
	ReflectedPropertyUndoRedoUtility::UndoRedoHelperList	undoRedoHelperList_;

	static int s_Connected;
};

#endif