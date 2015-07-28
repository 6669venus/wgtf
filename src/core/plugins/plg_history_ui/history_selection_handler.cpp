#include "history_selection_handler.hpp"
#include "variant/variant.hpp"
#include "variant/collection.hpp"


//==============================================================================
HistorySelectionHandler::HistorySelectionHandler()
{
}


//==============================================================================
HistorySelectionHandler::~HistorySelectionHandler()
{
}


//==============================================================================
void HistorySelectionHandler::setSelection( const Variant & selectionCollection )
{
	std::vector<unsigned int> selectionSet;
	bool isOk = selectionCollection.tryCast( selectionSet );
	if (!isOk)
	{
		assert( false );
		return;
	}
	selectionSet_ = selectionSet;
}


//==============================================================================
const std::vector<unsigned int> & HistorySelectionHandler::getSelection() const
{
	return selectionSet_;
}
