#include "context_object_manager.hpp"

ContextObjectManager::ContextObjectManager( IObjectManagerNew & parent, const wchar_t * contextName )
	: parent_( parent )
{

}