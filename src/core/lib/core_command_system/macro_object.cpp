#include "macro_object.hpp"
#include "core_command_system/i_command_manager.hpp"
#include <cassert>
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include <codecvt>
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

namespace RPURU = ReflectedPropertyUndoRedoUtility;


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
	, currentContextObj_( nullptr )
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
IListModel * MacroObject::getContextObjects() const
{
	if(!contextList_.empty())
	{
		return &contextList_;
	}
	std::vector< ObjectHandle > objs;
	pDefManager_->getObjectManager()->getObjects( objs );
	for (auto & obj : objs)
	{
		auto def = obj.getDefinition( *pDefManager_ );
		if (def == nullptr)
		{
			continue;
		}
		contextList_.push_back( obj );
	}
	return &contextList_;
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
}


//==============================================================================
ObjectHandle MacroObject::executeMacro() const
{
	assert( commandSystem_ != nullptr );
	if (macroEditObjectList_.empty())
	{
		createEditData();
	}
	ObjectHandle args = bindMacroArgumenets();
	CommandInstancePtr ins = commandSystem_->queueCommand( cmdId_.c_str(), args );
	currentContextObj_ = nullptr;
	return ins;
}


//==============================================================================
IListModel * MacroObject::createEditData() const
{
	//TODO-705: move display data to plg_macro_ui project
	if (!macroEditObjectList_.empty())
	{
		return &macroEditObjectList_;
	}

	assert( commandSystem_ != nullptr );
	assert( pDefManager_ != nullptr );
	CompoundCommand * macro = 
		static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert( macro != nullptr );
	auto & commands = macro->getSubCommands();
	assert( !commands.empty());
	int commandInstanceIndex = 0;
	for( auto & cmd : commands )
	{
		auto args = cmd.second.getBase<ReflectedPropertyCommandArgument>();
		assert( args != nullptr );
		auto editObject = pDefManager_->create<MacroEditObject>( false );
		editObject->subCommandIndex( commandInstanceIndex );
		editObject->propertyPath( args->getPropertyPath() );
		editObject->value( args->getPropertyValue() );
		macroEditObjectList_.push_back( editObject );
		commandInstanceIndex++;
	}
	return &macroEditObjectList_;
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
	if (macroEditObjectList_.empty())
	{
		createEditData();
	}

	for(VariantList::Iterator iter = macroEditObjectList_.begin(); iter != macroEditObjectList_.end(); ++iter)
	{
		const Variant & variant = *iter;
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
					RPURU::resolveContextObjectPropertyPath( currentContextObj_, args->getPropertyPath(), *pDefManager_ );
				args->setPath( propertyPath.c_str() );
			}
		}
	}
	return nullptr;
}

#include "core_variant/type_id.hpp"

template<typename T>
struct TypeCase
{
	typedef const std::function<void (T*)>& Func;
};

class EmptyType
{
public:
	static void empty(EmptyType*) {}
};

template<	typename T1,
					typename T2 = EmptyType,
					typename T3 = EmptyType,
					typename T4 = EmptyType,
					typename T5 = EmptyType >
class TypeSwitch
{
public:
	TypeSwitch( const ObjectHandle& objHandle ) : objHandle_(objHandle) { }

	bool match (	TypeCase<T1>::Func f1,
								TypeCase<T2>::Func f2 = EmptyType::empty,
								TypeCase<T3>::Func f3 = EmptyType::empty,
								TypeCase<T4>::Func f4 = EmptyType::empty,
								TypeCase<T5>::Func f5 = EmptyType::empty )
	{
		if (objHandle_.type() == TypeId::getType<T1>())
		{
			T1* p = objHandle_.getBase<T1>();
			f1(p);
			return true;
		}
		else
		{
			TypeSwitch<T2, T3, T4, T5>( objHandle_ ).match( f2, f3, f4, f5 );
		}
	}

private:
	const ObjectHandle& objHandle_;
};

template<>
class TypeSwitch<EmptyType>
{
	TypeSwitch( const ObjectHandle& objHandle ) { }

	bool match (	TypeCase<EmptyType>::Func f1,
		TypeCase<EmptyType>::Func f2 = EmptyType::empty,
		TypeCase<EmptyType>::Func f3 = EmptyType::empty,
		TypeCase<EmptyType>::Func f4 = EmptyType::empty,
		TypeCase<EmptyType>::Func f5 = EmptyType::empty )
	{
		return false;
	}
};

void mapping( ObjectHandle objHandle )
{

}

ObjectHandle MacroObject::bind( ReflectedPropertyCommandArgument* rpca, const Variant & variant) const
{
	ObjectHandleT<MacroEditObject> macroEdit;
	bool isOk = variant.tryCast( macroEdit );
	assert( isOk );

	auto argDef = pDefManager_->getDefinition<ReflectedPropertyCommandArgument>();
	assert( argDef != nullptr );
	ObjectHandle arg = argDef->create();
	ReflectedPropertyCommandArgument* clone = arg.getBase< ReflectedPropertyCommandArgument >();
	clone->setPath( macroEdit->propertyPath() );
	clone->setValue( macroEdit->value() );

	// update id and property path if context object selected
	if(currentContextObj_ != nullptr)
	{
		RefObjectId id;
		bool isOk = currentContextObj_.getId( id );
		if (isOk)
		{
			clone->setContextId( id );
			const std::string& propertyPath = 
				RPURU::resolveContextObjectPropertyPath( currentContextObj_, clone->getPropertyPath(), *pDefManager_ );
			clone->setPath( propertyPath.c_str() );
		}
	}

	return arg;
}

ObjectHandle MacroObject::bindMacroArgumenets() const
{
	assert( commandSystem_ != nullptr );
	auto argDef = pDefManager_->getDefinition<CompoundCommandArgument>();
	assert( argDef != nullptr );
	ObjectHandle args = argDef->create();
	CompoundCommandArgument* ccArgs = args.getBase< CompoundCommandArgument >();

	CompoundCommand * macro = 
		static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert( macro != nullptr );
	auto & commands = macro->getSubCommands();
	assert( !commands.empty());
	for (size_t i = 0; i < commands.size(); ++i)
	{
		bool mapped = TypeSwitch< ReflectedPropertyCommandArgument >( commands[i].second ).match(
			[&] (ReflectedPropertyCommandArgument* rpca) {
				ccArgs->setCommandArgument( i, bind( rpca, macroEditObjectList_[i] ) );
			}
		);

		if (!mapped)
		{
			ccArgs->setCommandArgument( i, commands[i].second );
		}
	}

	return args;
}
