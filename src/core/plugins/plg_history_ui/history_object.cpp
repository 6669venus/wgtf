#include "history_object.hpp"
#include "core_reflection/object_handle.hpp"
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

	pushHistoryItems( commandSystem_->getHistory() );

	using namespace std::placeholders;
	postHistoryItemsRemoved_ = historyItems_.onPostItemsRemoved.connect( std::bind( &HistoryObject::onPostHistoryItemsRemoved, this, _1, _2 ) );

	bindCommandHistoryCallbacks();
}

//==============================================================================
void HistoryObject::bindCommandHistoryCallbacks()
{
	commandSystem_->onHistoryPostInserted().add< HistoryObject,
		&HistoryObject::onPostCommandHistoryInserted >( this );
	commandSystem_->onHistoryPostRemoved().add< HistoryObject,
		&HistoryObject::onPostCommandHistoryRemoved >( this );
	commandSystem_->onHistoryPreReset().add< HistoryObject,
		&HistoryObject::onCommandHistoryPreReset >( this );
	commandSystem_->onHistoryPostReset().add< HistoryObject,
		&HistoryObject::onCommandHistoryPostReset >( this );
}

//==============================================================================
void HistoryObject::unbindCommandHistoryCallbacks()
{
	commandSystem_->onHistoryPostInserted().remove< HistoryObject,
		&HistoryObject::onPostCommandHistoryInserted >( this );
	commandSystem_->onHistoryPostRemoved().remove< HistoryObject,
		&HistoryObject::onPostCommandHistoryRemoved >( this );
	commandSystem_->onHistoryPreReset().remove< HistoryObject,
		&HistoryObject::onCommandHistoryPreReset >( this );
	commandSystem_->onHistoryPostReset().remove< HistoryObject,
		&HistoryObject::onCommandHistoryPostReset >( this );
}

//==============================================================================
void HistoryObject::pushHistoryItems( const VariantList& history )
{
	for(size_t i = 0; i < history.size(); i++)
	{
		auto displayObject = defManager_->create<DisplayObject>( false );
		auto instance = history[i].value<CommandInstancePtr>();
		displayObject->init( *defManager_, instance );
		historyItems_.push_back( displayObject );
	}
}

//==============================================================================
void HistoryObject::fini()
{
	assert( commandSystem_ != nullptr );

	unbindCommandHistoryCallbacks();

	postHistoryItemsRemoved_.disconnect();
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
void HistoryObject::onPostCommandHistoryInserted( const ICommandManager* sender,
																								const ICommandManager::HistoryPostInsertedArgs& args )
{
	assert( commandSystem_ != nullptr );
	assert( defManager_ != nullptr );
	assert( sender != nullptr );
	const VariantList & history = args.history_;
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
void HistoryObject::onPostCommandHistoryRemoved( const ICommandManager* sender,
																								const ICommandManager::HistoryPostRemovedArgs& args )
{
	assert( commandSystem_ != nullptr );
	assert( defManager_ != nullptr );
	assert( sender != nullptr );
	
	size_t index = args.index_;
	size_t count = args.count_;

	// detach listener to avoid event loop
	postHistoryItemsRemoved_.disable();
	historyItems_.erase( historyItems_.begin() + index, historyItems_.begin() + index + count );
	postHistoryItemsRemoved_.enable();

	assert( historyItems_.size() == args.history_.size() );
}

//==============================================================================
void HistoryObject::onCommandHistoryPreReset( const ICommandManager* sender,
																					const ICommandManager::HistoryPreResetArgs& args)
{
	postHistoryItemsRemoved_.disable();
	historyItems_.clear();
	postHistoryItemsRemoved_.enable();
}

//==============================================================================
void HistoryObject::onCommandHistoryPostReset( const ICommandManager* sender,
																						 const ICommandManager::HistoryPostResetArgs& args)
{
	postHistoryItemsRemoved_.disable();
	pushHistoryItems( args.history_ );
	postHistoryItemsRemoved_.enable();

	selectionHandler_.setSelectedRows( std::vector< int >() );
	selectionHandler_.setSelectedItems( std::vector< IItem* >() );
}

//==============================================================================
void HistoryObject::onPostHistoryItemsRemoved( size_t index, size_t count )
{
	// handling user click on clear button
	assert( commandSystem_ != nullptr );

	// detach listener to avoid event loop
	unbindCommandHistoryCallbacks();

	VariantList & history = const_cast<VariantList &>(commandSystem_->getHistory());
	history.erase( history.begin() + index, history.begin() + index + count );

	bindCommandHistoryCallbacks();

	assert( history.size() == historyItems_.size() );
	selectionHandler_.setSelectedRows( std::vector< int >() );
	selectionHandler_.setSelectedItems( std::vector< IItem* >() );
}
