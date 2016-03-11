#include "set_reflectedproperty_command.hpp"

#include "core_variant/variant.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

namespace RPURU = ReflectedPropertyUndoRedoUtility;
namespace
{
    void initReflectedMethodInDisplayObject(GenericObject& object, RPURU::ReflectedClassMemberUndoRedoHelper* helper)
    {
        object.set("Type", "Method");
        auto methodHelper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>(helper);
        size_t max = methodHelper->parameters_.size();
        char parameterName[32] = "Parameter";
        char * counterPointer = &parameterName[strlen(parameterName)];

        for (size_t i = 0; i < max; ++i)
        {
            sprintf(counterPointer, "%d", i);
            object.set(parameterName, methodHelper->parameters_[i]);
        }
    }

    //==========================================================================
    class PropertyAccessorWrapper
    : public PropertyAccessorListener
    {
    public:
        PropertyAccessorWrapper( RPURU::UndoRedoHelperList & undoRedoHelperList )
        : undoRedoHelperList_( undoRedoHelperList )
        {
        }
        
        ~PropertyAccessorWrapper()
        {
        }
        
        //======================================================================
        void preSetValue(
                         const PropertyAccessor & accessor, const Variant & value ) override
        {
            const auto & obj = accessor.getRootObject();
            assert( obj != nullptr );
            RefObjectId id;
            bool ok = obj.getId( id );
            if (!ok)
            {
                NGT_ERROR_MSG( "Trying to create undo/redo helper for unmanaged object\n" );
                // evgenys: we have to split notifications for managed and unmanaged objects
                return;
            }
            const char * propertyPath = accessor.getFullPath();
            const TypeId type = accessor.getType();
            Variant prevalue = accessor.getValue();
            auto pHelper = this->findUndoRedoHelper( id, propertyPath );
            if (pHelper != nullptr)
            {
                return;
            }
            auto helper = new RPURU::ReflectedPropertyUndoRedoHelper();
            helper->objectId_ = id;
            helper->path_ = propertyPath;
            helper->typeName_ = type.getName();
            helper->preValue_ = std::move( prevalue );
            undoRedoHelperList_.emplace_back( helper );
        }
        
        
        //======================================================================
        void postSetValue(
                          const PropertyAccessor & accessor, const Variant & value ) override
        {
            const auto & obj = accessor.getRootObject();
            assert( obj != nullptr );
            RefObjectId id;
            bool ok = obj.getId( id );
            if (!ok)
            {
                NGT_ERROR_MSG( "Trying to create undo/redo helper for unmanaged object\n" );
                // evgenys: we have to split notifications for managed and unmanaged objects
                return;
            }
            const char * propertyPath = accessor.getFullPath();
            Variant postValue = accessor.getValue();
            RPURU::ReflectedPropertyUndoRedoHelper* pHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(
                                                                                                                   this->findUndoRedoHelper( id, propertyPath ) );
            assert( pHelper != nullptr );
            pHelper->postValue_ = std::move( postValue );
        }
        
        
        void preInvoke(
                       const PropertyAccessor & accessor, const ReflectedMethodParameters& parameters, bool undo ) override
        {
            const char* path = accessor.getFullPath();
            const auto& object = accessor.getRootObject();
            assert( object != nullptr );
            
            RefObjectId id;
            bool ok = object.getId( id );
            assert(ok);
            
            RPURU::ReflectedMethodUndoRedoHelper* helper = static_cast<RPURU::ReflectedMethodUndoRedoHelper*>(
                                                                                                              this->findUndoRedoHelper( id, path ) );
            
            if (helper == nullptr)
            {
                auto helper = new RPURU::ReflectedMethodUndoRedoHelper();
                helper->objectId_ = id;
                helper->path_ = path;
                helper->parameters_ = parameters;
                undoRedoHelperList_.emplace_back( helper );
            }
            else
            {
                helper->parameters_ = parameters;
            }
        }
        
        
    private:
        RPURU::ReflectedClassMemberUndoRedoHelper* findUndoRedoHelper( 
                                                                      const RefObjectId & id, const char * propertyPath )
        {
            RPURU::ReflectedClassMemberUndoRedoHelper* helper = nullptr;
            for (auto& findIt : undoRedoHelperList_)
            {
                if (findIt->objectId_ == id && findIt->path_ == propertyPath)
                {
                    helper = findIt.get();
                    break;
                }
            }
            return helper;
        }
    private:
        RPURU::UndoRedoHelperList &	undoRedoHelperList_;
    };
    
}

//==============================================================================
const char * ReflectedPropertyCommandArgument::s_ContextId = "PropertyContextId";
const char * ReflectedPropertyCommandArgument::s_PropertyPath = "PropertyPath";
const char * ReflectedPropertyCommandArgument::s_PropertyValue = "PropertyValue";


//==============================================================================
const char * ReflectedPropertyCommandArgument::contextIdPropertyName()
{
	return s_ContextId;
}


//==============================================================================
const char * ReflectedPropertyCommandArgument::pathPropertyName()
{
	return s_PropertyPath;
}


//==============================================================================
const char * ReflectedPropertyCommandArgument::valuePropertyName()
{
	return s_PropertyValue;
}

//==============================================================================
ReflectedPropertyCommandArgument::ReflectedPropertyCommandArgument()
	: contextId_("")
	, propertyPath_("")
{
}

//==============================================================================
const RefObjectId & ReflectedPropertyCommandArgument::getContextId() const
{
	return contextId_;
}


//==============================================================================
const char * ReflectedPropertyCommandArgument::getPropertyPath() const
{
	return propertyPath_.c_str();
}


//==============================================================================
const Variant & ReflectedPropertyCommandArgument::getPropertyValue() const
{
	return value_;
}


//==============================================================================
void ReflectedPropertyCommandArgument::setContextId( const RefObjectId & id )
{
	contextId_ = id;
}


//==============================================================================
void ReflectedPropertyCommandArgument::setPath( const char * path )
{
	propertyPath_ = path;
}


//==============================================================================
void ReflectedPropertyCommandArgument::setValue( const Variant & value )
{
	value_ = value;
}

typedef XMLSerializer UndoRedoSerializer;

void ReflectedPropertyCommandArgument::connectEvents(IDefinitionManager * definitionManager)
{
    if (paListener_ == nullptr)
    {
        paListener_ = std::make_shared< PropertyAccessorWrapper >( undoRedoHelperList_ );
    }
    
    assert( definitionManager != nullptr );
    definitionManager->registerPropertyAccessorListener( paListener_ );
}

void ReflectedPropertyCommandArgument::disconnectEvents(IDefinitionManager * definitionManager)
{
    assert( definitionManager != nullptr );
    definitionManager->deregisterPropertyAccessorListener( paListener_ );
    
    UndoRedoSerializer undoSerializer( undoData_, *definitionManager );
    undoSerializer.serialize( RPURU::getUndoStreamHeaderTag() );
    undoSerializer.serialize( undoRedoHelperList_.size() );
    
    UndoRedoSerializer redoSerializer( redoData_, *definitionManager );
    redoSerializer.serialize( RPURU::getRedoStreamHeaderTag() );
    redoSerializer.serialize( undoRedoHelperList_.size() );
    
    for (const auto& helper : undoRedoHelperList_)
    {
        RPURU::saveUndoData( undoSerializer, *helper );
        RPURU::saveRedoData( redoSerializer, *helper );
    }
    
    undoRedoHelperList_.clear();
}

//==============================================================================
SetReflectedPropertyCommand::SetReflectedPropertyCommand( IDefinitionManager & definitionManager )
	: definitionManager_( definitionManager )
{
}


//==============================================================================
SetReflectedPropertyCommand::~SetReflectedPropertyCommand()
{
}


//==============================================================================
const char * SetReflectedPropertyCommand::getId() const
{
	static const char * s_Id = getClassIdentifier<SetReflectedPropertyCommand>();
	return s_Id;
}


//==============================================================================
ObjectHandle SetReflectedPropertyCommand::execute(
	const ObjectHandle & arguments ) const
{
	ReflectedPropertyCommandArgument * commandArgs =
		arguments.getBase< ReflectedPropertyCommandArgument >();
	auto objManager = definitionManager_.getObjectManager();
	assert( objManager != nullptr );
	const ObjectHandle & object = objManager->getObject( commandArgs->getContextId() );
	PropertyAccessor property = object.getDefinition( definitionManager_ )->bindProperty( 
		commandArgs->getPropertyPath(), object );
	if (property.isValid() == false)
	{
		//Can't set
		return CommandErrorCode::INVALID_ARGUMENTS;
	}
	const Variant & data = commandArgs->getPropertyValue();
	bool br = property.setValue( data );
	if (!br)
	{
		return CommandErrorCode::INVALID_VALUE;
	}
	return nullptr;
}


//==============================================================================
CommandThreadAffinity SetReflectedPropertyCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}

void SetReflectedPropertyCommand::undo( const ObjectHandle & arguments ) const
{
    ReflectedPropertyCommandArgument * args = arguments.getBase<ReflectedPropertyCommandArgument>();
    assert(args != nullptr);
    const auto pObjectManager = definitionManager_.getObjectManager();
    assert( pObjectManager != nullptr );
    
    ResizingMemoryStream & undoData = args->getUndoStream();
    if (!undoData.buffer().empty())
    {
        undoData.seek( 0 );
        UndoRedoSerializer serializer( undoData, definitionManager_ );
        RPURU::performReflectedUndo( serializer, *pObjectManager, definitionManager_ );
    }
}

void SetReflectedPropertyCommand::redo( const ObjectHandle & arguments ) const 
{
    ReflectedPropertyCommandArgument * args = arguments.getBase<ReflectedPropertyCommandArgument>();
    assert(args != nullptr);
    const auto pObjectManager = definitionManager_.getObjectManager();
    assert( pObjectManager != nullptr );
    ResizingMemoryStream & redoData = args->getRedoStream();
    if (!redoData.buffer().empty())
    {
        redoData.seek( 0 );
        UndoRedoSerializer serializer( redoData, definitionManager_ );
        RPURU::performReflectedRedo( serializer, *pObjectManager, definitionManager_ );
    }
}

ObjectHandle SetReflectedPropertyCommand::getCommandDescription(const ObjectHandle & arguments) const
{
    auto pObjectManager = definitionManager_.getObjectManager();
    assert(pObjectManager != nullptr);
    auto& objectManager = (*pObjectManager);
    const char * undoStreamHeaderTag = RPURU::getUndoStreamHeaderTag();
    const char * redoStreamHeaderTag = RPURU::getRedoStreamHeaderTag();
    ReflectedPropertyCommandArgument * args = arguments.getBase<ReflectedPropertyCommandArgument>();
    if (args == nullptr)
        return ObjectHandle();

    const ResizingMemoryStream & undoData = args->getUndoStream();
    const ResizingMemoryStream & redoData = args->getRedoStream();
    ObjectHandle result;
    RPURU::UndoRedoHelperList propertyCache;
    {
        // Need to read undo/redo data separately and then consolidate it into
        // propertyCache.

        // Make a copy because this function should not modify stream contents
        // TODO ResizingMemoryStream const read implementation
        ResizingMemoryStream undoStream(undoData.buffer());
        assert(!undoStream.buffer().empty());
        CommandInstance::UndoRedoSerializer undoSerializer(undoStream, definitionManager_);

        // Read property header
        std::string undoHeader;
        undoHeader.reserve(strlen(undoStreamHeaderTag));
        undoSerializer.deserialize(undoHeader);
        assert(undoHeader == undoStreamHeaderTag);

        // Make a copy because this function should not modify stream contents
        // TODO ResizingMemoryStream const read implementation
        ResizingMemoryStream redoStream(redoData.buffer());
        assert(!redoStream.buffer().empty());
        CommandInstance::UndoRedoSerializer redoSerializer(redoStream, definitionManager_);

        // Read property header
        std::string redoHeader;
        redoHeader.reserve(strlen(redoStreamHeaderTag));
        redoSerializer.deserialize(redoHeader);
        assert(redoHeader == redoStreamHeaderTag);

        // Read properties into cache
        const bool reflectedPropertiesLoaded = loadReflectedProperties(
            propertyCache,
            undoSerializer,
            redoSerializer,
            objectManager,
            definitionManager_);
        if (!reflectedPropertiesLoaded)
        {
            return ObjectHandle();
        }
    }

    // Create display object from cache
    {
        auto handle = GenericObject::create(definitionManager_);
        assert(handle.get() != nullptr);
        auto& genericObject = (*handle);

        // command with no property change
        if (propertyCache.empty())
        {
            genericObject.set("Name", "Unknown");
            genericObject.set("Type", "Unknown");
        }
        // Single command
        else if (propertyCache.size() == 1)
        {
            auto& helper = propertyCache.at(0);
            // TODO: Refactor this and the section below as they do the same thing.
            genericObject.set("Id", helper->objectId_);
            ObjectHandle object = objectManager.getObject(helper->objectId_);

            if (object == nullptr)
            {
                genericObject.set("Name", helper->path_);
            }
            else
            {
                assert(object != nullptr);
                PropertyAccessor pa(object.getDefinition(definitionManager_)->bindProperty(
                    helper->path_.c_str(), object));
                auto metaData = findFirstMetaData< MetaInPlacePropertyNameObj >(pa, definitionManager_);
                if (metaData != nullptr)
                {
                    const char * propName = metaData->getPropertyName();
                    pa = object.getDefinition(definitionManager_)->bindProperty(propName, object);
                    auto value = pa.getValue();
                    std::string name;
                    bool isOk = value.tryCast(name);
                    assert(isOk);
                    genericObject.set("Name", name);
                }
                else
                {
                    genericObject.set("Name", helper->path_);
                }
            }

            if (helper->isMethod())
            {
                initReflectedMethodInDisplayObject(genericObject, helper.get());
            }
            else
            {
                auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());
                genericObject.set("Type", propertyHelper->typeName_);
                genericObject.set("PreValue", propertyHelper->preValue_);
                genericObject.set("PostValue", propertyHelper->postValue_);
            }
        }
        else
        {
            genericObject.set("Name", "Batch");
            genericObject.set("Type", "Batch");

            // Need to create a CollectionHolder, otherwise
            // genericObject.set( "Children", children );
            // is unsafe, because it takes a reference
            // which will be deleted when children goes out of scope
            typedef std::vector< GenericObjectPtr > Children;
            auto collectionHolder =
                std::make_shared< CollectionHolder< Children > >();

            Children& children = collectionHolder->storage();
            children.reserve(propertyCache.size());

            for (const auto& helper : propertyCache)
            {
                auto childHandle = GenericObject::create(definitionManager_);
                assert(childHandle.get() != nullptr);

                auto& childObject = (*childHandle);
                // TODO: Refactor this and the section above as they do the same thing.
                childObject.set("Id", helper->objectId_);
                ObjectHandle object = objectManager.getObject(helper->objectId_);

                if (object == nullptr)
                {
                    genericObject.set("Name", helper->path_);
                }
                else
                {
                    PropertyAccessor pa(object.getDefinition(definitionManager_)->bindProperty(
                        helper->path_.c_str(), object));
                    auto metaData = findFirstMetaData< MetaInPlacePropertyNameObj >(pa, definitionManager_);
                    if (metaData != nullptr)
                    {
                        const char * propName = metaData->getPropertyName();
                        pa = object.getDefinition(definitionManager_)->bindProperty(propName, object);
                        auto value = pa.getValue();
                        std::string name;
                        bool isOk = value.tryCast(name);
                        assert(isOk);
                        childObject.set("Name", name);
                    }
                    else
                    {
                        genericObject.set("Name", helper->path_);
                    }
                }

                if (helper->isMethod())
                {
                    initReflectedMethodInDisplayObject(genericObject, helper.get());
                }
                else
                {
                    auto propertyHelper = static_cast<RPURU::ReflectedPropertyUndoRedoHelper*>(helper.get());
                    childObject.set("Type", propertyHelper->typeName_);
                    childObject.set("PreValue", propertyHelper->preValue_);
                    childObject.set("PostValue", propertyHelper->postValue_);
                }

                children.push_back(childHandle);
            }

            // Convert CollectionHolder to Collection
            Collection childrenCollection(collectionHolder);
            genericObject.set("Children", childrenCollection);
        }

        result = ObjectHandle(std::move(handle));
    }

    return result;
}
