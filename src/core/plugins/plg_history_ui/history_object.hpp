#ifndef HISTORY_OBJECT_HPP
#define HISTORY_OBJECT_HPP

#include "reflection/object_handle.hpp"


class CommandSystemProvider;


/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class HistoryObject
{
public:
	// ClassDefinition::create() requires a default constructor
	HistoryObject();
	void init( CommandSystemProvider& commandSystem );

	ObjectHandle getHistory() const;
	ObjectHandle currentIndexSource() const;
	const int & currentSelectedRowIndex() const;
	void currentSelectedRowIndex( const int & index );
	ObjectHandle createMacro() const;
private:
	CommandSystemProvider* commandSystem_;
	int currentSelectedRowIndex_;
};


#endif // HISTORY_OBJECT_HPP
