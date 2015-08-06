#include "macro_object.hpp"
#include "command_system/i_command_manager.hpp"
#include <cassert>
#include "reflection/i_definition_manager.hpp"
#include "reflection/i_object_manager.hpp"
#include "serialization/serializer/i_serialization_manager.hpp"
#include "command_system/compound_command.hpp"
#include "data_model/generic_list.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "string_utils/string_utils.hpp"
#include <codecvt>
#include "reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

namespace RPURU = ReflectedPropertyUndoRedoUtility;

//==============================================================================
class ContextObjectListItem : public GenericListItem
{
public:
	ContextObjectListItem( const Variant& value )
		: GenericListItem( value )
		, displayName_( "Unknown" )
	{

	}
	ContextObjectListItem( Variant&& value )
		: GenericListItem( std::forward<Variant&&>( value ) )
		, displayName_( "Unknown" )
	{

	}
	// IItem
	const char * getDisplayText( int column ) const override
	{
		const Variant & value = this->value<const Variant &>();
		if (value.typeIs<ObjectHandle>())
		{
			ObjectHandle objHandle;
			bool isOk = value.tryCast( objHandle );
			assert( isOk );
			auto metaName =
				findFirstMetaData< MetaDisplayNameObj >( *objHandle.getDefinition() );
			if (metaName != nullptr)
			{
				std::wstring_convert< Utf16to8Facet > conversion( 
					Utf16to8Facet::create() );

				displayName_ = conversion.to_bytes( metaName->getDisplayName() );
			}
			else
			{
				const auto classDef = objHandle.getDefinition();
				if (classDef != nullptr)
				{
					if (classDef->isGeneric())
					{
						displayName_ = "GenericObject";
					}
					else
					{
						displayName_ = classDef->getName();
					}
				}
			}
		}
		return displayName_.c_str();
	}
private:
	//TODO: http://jira.bigworldtech.com/browse/NGT-434
	mutable std::string displayName_;
};


//==============================================================================
MacroEditObject::MacroEditObject()
	: subCommandIndex_( -1 )
	, propertyPath_( "" )
{

}


//==============================================================================
const int & MacroEditObject::subCommandIndex() const
{
	return subCommandIndex_;
}


//==============================================================================
const char * MacroEditObject::propertyPath() const
{
	return propertyPath_.c_str();
}

//==============================================================================
const Variant & MacroEditObject::value() const
{
	return value_;
}


//==============================================================================
void MacroEditObject::subCommandIndex( const int & index)
{
	subCommandIndex_ = index;
}


//==============================================================================
void MacroEditObject::propertyPath( const char * propertyPath )
{
	propertyPath_ = propertyPath;
}


//==============================================================================
void MacroEditObject::value( const Variant & value )
{
	value_ = value;
}


//==============================================================================
MacroObject::MacroObject()
	: commandSystem_( nullptr )
	, pDefManager_( nullptr )
	, cmdId_( "" )
	, macroName_( "" )
	, contextList_( nullptr )
	, currentContextObj_( nullptr )
	, macroEditObjectList_( nullptr )
{
}


//==============================================================================
void MacroObject::init( ICommandManager& commandSystem, IDefinitionManager & defManager, const char * cmdId )
{
	commandSystem_ = &commandSystem;
	pDefManager_ = &defManager;
	cmdId_ = cmdId;
	macroName_ = cmdId_;
}


//==============================================================================
const ObjectHandle & MacroObject::getContextObjects() const
{
	if(contextList_ != nullptr)
	{
		return contextList_;
	}
	std::unique_ptr<GenericList> objList( new GenericList() );
	std::vector< ObjectHandle > objs;
	pDefManager_->getObjectManager()->getObjects( objs );
	for (auto & obj : objs)
	{
		auto def = obj.getDefinition();
		if (def == nullptr)
		{
			continue;
		}
		std::unique_ptr<ContextObjectListItem> item( new ContextObjectListItem( obj ) );
		objList->push_back( item.release() );
	}
	contextList_ = std::move( objList );
	return contextList_;
}


//==============================================================================
const ObjectHandle & MacroObject::getContextObject() const
{
	return currentContextObj_;
}

//==============================================================================
void MacroObject::setContextObject( const ObjectHandle & obj )
{
	// move context setting into individual args setting
	currentContextObj_ = obj;
	this->updateMacro();
}


//==============================================================================
ObjectHandle MacroObject::executeMacro() const
{
	assert( commandSystem_ != nullptr );
	auto argDef = pDefManager_->getDefinition<CompoundCommandArgument>();
	assert( argDef != nullptr );
	CommandInstancePtr ins = commandSystem_->queueCommand( cmdId_.c_str(), nullptr );
	return ins;
}


//==============================================================================
ObjectHandle MacroObject::createEditData() const
{
	//TODO-705: move display data to plg_macro_ui project
	if (macroEditObjectList_ != nullptr)
	{
		return macroEditObjectList_;
	}

	assert( commandSystem_ != nullptr );
	assert( pDefManager_ != nullptr );
	CompoundCommand * macro = 
		static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert( macro != nullptr );
	std::unique_ptr<GenericList> objList( new GenericList() );
	auto & commands = macro->getSubCommands();
	assert( !commands.empty());
	int commandInstanceIndex = 0;
	for( auto & cmd : commands )
	{
		auto args = cmd.second.getBase<ReflectedPropertyCommandArgument>();
		assert( args != nullptr );
		auto editObject = pDefManager_->createT<MacroEditObject>( false );
		editObject->subCommandIndex( commandInstanceIndex );
		editObject->propertyPath( args->getPropertyPath() );
		editObject->value( args->getPropertyValue() );
		objList->push_back( editObject );
		commandInstanceIndex++;
	}
	macroEditObjectList_ = std::move( objList );
	return macroEditObjectList_;
}

//==============================================================================
ObjectHandle MacroObject::updateMacro() const
{
	assert( commandSystem_ != nullptr );
	CompoundCommand * macro = 
		static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert( macro != nullptr );
	auto & commands = macro->getSubCommands();
	assert( !commands.empty());
	size_t count = commands.size();

	// write data to the stream
	if (macroEditObjectList_ == nullptr)
	{
		createEditData();
	}
	GenericList* objList = macroEditObjectList_.getBase<GenericList>();
	for(GenericList::Iterator iter = objList->begin(); iter != objList->end(); ++iter)
	{
		const Variant & variant = (*iter).value<const Variant &>();
		ObjectHandleT<MacroEditObject> obj;
		bool isOk = variant.tryCast( obj );
		assert( isOk );
		size_t index = obj->subCommandIndex();
		assert( index < count );
		auto args = commands[index].second.getBase<ReflectedPropertyCommandArgument>();
		assert( args != nullptr );
		args->setPath( obj->propertyPath() );
		args->setValue( obj->value() );
		
		// update id and property path if context object selected
		if(currentContextObj_ != nullptr)
		{
			RefObjectId id;
			bool isOk = currentContextObj_.getId( id );
			if (isOk)
			{
				args->setContextId( id );
				std::string propertyPath = 
					RPURU::resolveContextObjectPropertyPath( currentContextObj_, 
					args->getPropertyPath() );
				args->setPath( propertyPath.c_str() );
			}
		}
	}
	return nullptr;
}
