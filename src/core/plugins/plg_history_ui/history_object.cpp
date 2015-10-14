#include "history_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_data_model/i_value_change_notifier.hpp"
#include "core_logging/logging.hpp"
#include "display_object.hpp"
#include <cassert>


//==============================================================================
HistoryObject::HistoryObject()
	: commandSystem_( nullptr )
{
}

//==============================================================================
void HistoryObject::init( ICommandManager& commandSystem, IDefinitionManager& defManager )
{
	commandSystem_ = &commandSystem;
	defManager_ = &defManager;
	std::unique_ptr< VariantList > historyList( new VariantList() );
	VariantList & history = const_cast<VariantList&>(commandSystem_->getHistory());
	for(size_t i = 0; i < history.size(); i++)
	{
		auto displayObject = defManager.create<DisplayObject>( false );
		auto instance = history[i].value<CommandInstancePtr>();
		displayObject->init( defManager, instance );
		historyItems_.push_back( displayObject );
	}
	historyItems_.onPostItemsRemoved().add<HistoryObject, 
		&HistoryObject::onPostHistoryItemsRemoved>( this );

	history.onPostItemsInserted().add< HistoryObject,
		&HistoryObject::onPostCommandHistoryInserted >( this );
	history.onPostItemsRemoved().add< HistoryObject,
		&HistoryObject::onPostCommandHistoryRemoved >( this );

	
}

//==============================================================================
void HistoryObject::fini()
{
	assert( commandSystem_ != nullptr );
	VariantList & history = const_cast<VariantList&>(commandSystem_->getHistory());
	history.onPostItemsInserted().remove< HistoryObject,
		&HistoryObject::onPostCommandHistoryInserted >( this );
	history.onPostItemsRemoved().remove< HistoryObject,
		&HistoryObject::onPostCommandHistoryRemoved >( this );
	historyItems_.onPostItemsRemoved().remove<HistoryObject, 
		&HistoryObject::onPostHistoryItemsRemoved>( this );
}

//==============================================================================
const IListModel * HistoryObject::getHistory() const
{
	return &historyItems_;
}

//==============================================================================
const IValueChangeNotifier * HistoryObject::currentIndexSource() const
{
	return &commandSystem_->currentIndex();
}

//==============================================================================
const ISelectionHandler * HistoryObject::selectionHandlerSource() const
{
	return &selectionHandler_;
}

//==============================================================================
ObjectHandle HistoryObject::createMacro() const
{
	assert( commandSystem_ != nullptr );
	const VariantList & history = commandSystem_->getHistory();
	int size = static_cast<int>(history.size());
	VariantList commandList;
	const auto & selectionSet = selectionHandler_.getSelectedRows();
	if (selectionSet.empty())
	{
		return nullptr;
	}
	for( auto index : selectionSet)
	{
		if (index >= size)
		{
			assert( false );
			return nullptr;
		}
		commandList.push_back(history[index]);
	}
	commandSystem_->createMacro( commandList );
	return nullptr;
}

//==============================================================================
void HistoryObject::onPostCommandHistoryInserted( const IListModel* sender, 
						 const IListModel::PostItemsInsertedArgs& args )
{
	assert( commandSystem_ != nullptr );
	assert( defManager_ != nullptr );
	assert( sender != nullptr );
	const VariantList & history = *static_cast< const VariantList * >( sender );
	size_t historySize = history.size();
	size_t index = args.index_;
	size_t count = args.count_;
	historyItems_.resize( index );
	for (size_t i = 0; i < count; i++)
	{
		assert( index < historySize );
		auto displayObject = defManager_->create<DisplayObject>( false );
		auto instance = history[index++].value<CommandInstancePtr>();
		displayObject->init( *defManager_, instance );
		historyItems_.push_back( displayObject );
	}
	assert( historySize == historyItems_.size() );
}

//==============================================================================
void HistoryObject::onPostCommandHistoryRemoved( const IListModel* sender, 
						const IListModel::PostItemsRemovedArgs& args )
{
	assert( commandSystem_ != nullptr );
	assert( defManager_ != nullptr );
	assert( sender != nullptr );
	
	size_t index = args.index_;
	size_t count = args.count_;
	// detach listener to avoid event loop
	historyItems_.onPostItemsRemoved().remove<HistoryObject, 
		&HistoryObject::onPostHistoryItemsRemoved>( this );
	historyItems_.erase( historyItems_.begin() + index, historyItems_.begin() + index + count );
	historyItems_.onPostItemsRemoved().add<HistoryObject, 
		&HistoryObject::onPostHistoryItemsRemoved>( this );
	assert( historyItems_.size() == sender->size() );
}


void HistoryObject::onPostHistoryItemsRemoved( const IListModel* sender, 
							   const IListModel::PostItemsRemovedArgs& args )
{
	// handling user click on clear button
	assert( commandSystem_ != nullptr );
	size_t index = args.index_;
	size_t count = args.count_;
	VariantList & history = const_cast<VariantList &>(commandSystem_->getHistory());
	// detach listener to avoid event loop
	history.onPostItemsInserted().remove< HistoryObject,
		&HistoryObject::onPostCommandHistoryInserted >( this );
	history.onPostItemsRemoved().remove< HistoryObject,
		&HistoryObject::onPostCommandHistoryRemoved >( this );
	history.erase( history.begin() + index, history.begin() + index + count );
	history.onPostItemsInserted().add< HistoryObject,
		&HistoryObject::onPostCommandHistoryInserted >( this );
	history.onPostItemsRemoved().add< HistoryObject,
		&HistoryObject::onPostCommandHistoryRemoved >( this );
	assert( history.size() == historyItems_.size() );
	selectionHandler_.setSelectedRows( std::vector< int >() );
	selectionHandler_.setSelectedItems( std::vector< IItem* >() );
}
