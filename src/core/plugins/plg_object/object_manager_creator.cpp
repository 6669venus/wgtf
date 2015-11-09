#include "object_manager_creator.hpp"

#include "context_object_manager.hpp"
#include "object_manager.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

class ObjectManagerCreator::Impl
{
public:
	ObjectManager objectManager_;
};

ObjectManagerCreator::ObjectManagerCreator( IComponentContext & componentContext )
	: impl_( new Impl() )
{
	componentContext.registerInterface( &impl_->objectManager_ );
}

ObjectManagerCreator::~ObjectManagerCreator()
{

}

const char * ObjectManagerCreator::getType() const
{
	return typeid( ContextObjectManager ).name();
}

IInterface * ObjectManagerCreator::createContext( const wchar_t * contextId )
{
	auto context = new ContextObjectManager( impl_->objectManager_, contextId );
	return new InterfaceHolder< ContextObjectManager >( context, true );
}