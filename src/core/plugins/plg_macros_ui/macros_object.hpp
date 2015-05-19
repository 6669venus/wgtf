#ifndef MACROS_OBJECT_HPP
#define MACROS_OBJECT_HPP

#include "reflection/object_handle.hpp"


class CommandSystemProvider;

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class MacrosObject
{
public:
	// ClassDefinition::create() requires a default constructor
	MacrosObject();
	void init( CommandSystemProvider& commandSystem );

	ObjectHandle getMacros() const;
	ObjectHandle getSelectedCompoundCommand() const;
	const int & currentSelectedRowIndex() const;
	void currentSelectedRowIndex( const int & index );

private:
	
	CommandSystemProvider* commandSystem_;
	int currentSelectedRowIndex_;
};


#endif // MACROS_OBJECT_HPP
