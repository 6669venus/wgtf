#include "singleton_manager.hpp"

namespace BW
{

std::auto_ptr< SingletonManager > SingletonManager::s_pInstance( new SingletonManager() );

}
