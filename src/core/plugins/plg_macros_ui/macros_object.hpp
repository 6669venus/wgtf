#ifndef MACROS_OBJECT_HPP
#define MACROS_OBJECT_HPP

#include "core_reflection/object_handle.hpp"
#include "core_data_model/selection_handler.hpp"

class ICommandManager;

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class MacrosObject
{
public:
	// ClassDefinition::create() requires a default constructor
	MacrosObject();
	void init( ICommandManager& commandSystem );

	ObjectHandle getMacros() const;
	ObjectHandle getSelectedCompoundCommand() const;
	ObjectHandle selectionHandlerSource() const;

private:
	
	ICommandManager* commandSystem_;
	// TODO: http://jira.bigworldtech.com/browse/NGT-849
	// Eventually, we need to remove this
	SelectionHandler selectionHandler;
};


#endif // MACROS_OBJECT_HPP
