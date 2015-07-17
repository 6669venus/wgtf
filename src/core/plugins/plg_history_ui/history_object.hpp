#ifndef HISTORY_OBJECT_HPP
#define HISTORY_OBJECT_HPP


#include "data_model/i_list_model.hpp"
#include "reflection/object_handle.hpp"
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
	const int & currentSelectedRowIndex() const;
	void currentSelectedRowIndex( const int & index );
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
	int currentSelectedRowIndex_;
	ObjectHandle historyItems_;
};


#endif // HISTORY_OBJECT_HPP
