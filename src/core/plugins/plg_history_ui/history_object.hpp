#ifndef HISTORY_OBJECT_HPP
#define HISTORY_OBJECT_HPP


#include "core_data_model/i_list_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/selection_handler.hpp"
class ICommandManager;
class IDefinitionManager;

/**
 *	Wrapper for accessing the current position in the undo/redo list from QML.
 */
class HistoryObject
{
public:
	// ClassDefinition::create() requires a default constructor
	HistoryObject();
	void init( ICommandManager& commandSystem, IDefinitionManager& defManager );
	void fini();

	ObjectHandle getHistory() const;
	ObjectHandle currentIndexSource() const;
	ObjectHandle selectionHandlerSource() const;
	ObjectHandle createMacro() const;
private:
	void onPostCommandHistoryInserted( const IListModel* sender, 
		const IListModel::PostItemsInsertedArgs& args );
	void onPostCommandHistoryRemoved( const IListModel* sender, 
		const IListModel::PostItemsRemovedArgs& args );

	void onPostHistoryItemsRemoved( const IListModel* sender, 
		const IListModel::PostItemsRemovedArgs& args );


	ICommandManager* commandSystem_;
	IDefinitionManager* defManager_;
	// TODO: http://jira.bigworldtech.com/browse/NGT-849
	// Eventually, we need to remove this
	SelectionHandler selectionHandler_;
	ObjectHandle historyItems_;
};


#endif // HISTORY_OBJECT_HPP
