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
void HistorySelectionHandler::setSelection(const Variant & selectionSet )
{

}


//==============================================================================
const std::set<unsigned int> & HistorySelectionHandler::getSelection() const
{
	return selectionSet_;
}
