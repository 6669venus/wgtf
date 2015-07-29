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
void HistorySelectionHandler::setSelection( const std::vector<unsigned int> & selectionCollection )
{
	selectionSet_ = selectionCollection;
}


//==============================================================================
const std::vector<unsigned int> & HistorySelectionHandler::getSelection() const
{
	return selectionSet_;
}
