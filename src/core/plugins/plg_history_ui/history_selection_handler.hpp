#ifndef HISTORY_SELECTION_HANDLER_HPP
#define HISTORY_SELECTION_HANDLER_HPP

#include <vector>
class Variant;
class Collection;

class HistorySelectionHandler
{
public:
    HistorySelectionHandler();
    ~HistorySelectionHandler();

	void setSelection( const Variant & selectionCollection );
	const std::vector<unsigned int> & getSelection() const;

private:
	std::vector<unsigned int> selectionSet_;
};


#endif //HISTORY_SELECTION_HANDLER_HPP
