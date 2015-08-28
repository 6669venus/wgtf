#ifndef I_SELECTION_HANDLER_HPP
#define I_SELECTION_HANDLER_HPP

#include <vector>
#include "wg_types/event.hpp"
class IItem;

// TODO: http://jira.bigworldtech.com/browse/NGT-849
// Eventually, we need to remove this class

class ISelectionHandler
{
public:
	virtual ~ISelectionHandler() {}

	virtual void setSelectedItems( const std::vector< IItem* > & selectionCollection ) = 0;
	virtual const std::vector< IItem* > & getSelectedItems() const = 0;
	virtual void setSelectedRows( const std::vector< int > & selectionCollection ) = 0;
	virtual const std::vector< int > & getSelectedRows() const = 0;
	
	PUBLIC_EVENT( ISelectionHandler, PreSelectionChanged )
	PUBLIC_EVENT( ISelectionHandler, PostSelectionChanged )
};


#endif //I_SELECTION_HANDLER_HPP
