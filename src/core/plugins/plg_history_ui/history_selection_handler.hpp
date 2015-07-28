#ifndef HISTORY_SELECTION_HANDLER_HPP
#define HISTORY_SELECTION_HANDLER_HPP

#include <set>
class Variant;

class HistorySelectionHandler
{
public:
    HistorySelectionHandler();
    ~HistorySelectionHandler();

	void setSelection(const Variant & selectionSet );
	const std::set<unsigned int> & getSelection() const;

private:
	std::set<unsigned int> selectionSet_;
};


#endif //HISTORY_SELECTION_HANDLER_HPP
