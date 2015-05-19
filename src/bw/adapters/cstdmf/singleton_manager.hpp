#ifndef SINGLETON_MANAGER_HPP
#define SINGLETON_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <memory>

#define SINGLETON_MANAGER_WRAPPER( Type )\
	BW::SingletonManager::InstanceFunc pFunc;\
	if (BW::SingletonManager::instance().isRegisteredFunc< Type >( pFunc, &Type::instance ) == false)\
	{\
	return BW::SingletonManager::instance().executeFunc( pFunc, &Type::instance, &Type::instance );\
	}

#define SINGLETON_MANAGER_WRAPPER_FUNC( Type, Func )\
	BW::SingletonManager::InstanceFunc pFunc;\
	if (BW::SingletonManager::instance().isRegisteredFunc< Type >( pFunc, Func ) == false)\
	{\
	return BW::SingletonManager::instance().executeFunc( pFunc, Func, Func );\
	}

#define REGISTER_SINGLETON( Type )\
	{\
	BW::SingletonManager::InstanceFunc pFunc;\
	if (BW::SingletonManager::instance().isRegisteredFunc< Type >( pFunc, &Type::instance ))\
		{\
		BW::SingletonManager::instance().registerInstanceFunc< Type >( &Type::instance );\
		}\
	}

#define REGISTER_SINGLETON_FUNC( Type, Func )\
	{\
	BW::SingletonManager::InstanceFunc pFunc;\
	if (BW::SingletonManager::instance().isRegisteredFunc< Type >( pFunc, Func ))\
		{\
		BW::SingletonManager::instance().registerInstanceFunc< Type >( Func );\
		}\
	}

#define DEREGISTER_SINGLETON_FUNC( Type )\
	BW::SingletonManager::instance().deregisterInstanceFunc< Type >();

namespace BW
{

class SingletonManager
{
public:
	typedef int & (*InstanceFunc)();

	SingletonManager()
	{
		s_pInstance.reset( this );
	}

	template< class T, typename F >
	void registerInstanceFunc( F func )
	{
		s_Singletons.insert(
			std::make_pair(
				typeid( T ).name(),
				reinterpret_cast< InstanceFunc >( func ) ) );
	}

	template< class T >
	void deregisterInstanceFunc()
	{
		s_Singletons.erase( typeid( T ).name() );
	}

	template< class T, typename U >
	bool isRegisteredFunc( InstanceFunc & pFunc, U currFunc )
	{
		CollectionType::iterator instanceFuncIt =
			s_Singletons.find( typeid( T ).name() );
		if (instanceFuncIt == s_Singletons.end())
		{
			return true;
		}
		pFunc = instanceFuncIt->second;
		return reinterpret_cast< U >( pFunc ) == currFunc;
	}
 
	template< typename ReturnType, typename U >
	ReturnType executeFunc(
		InstanceFunc foundFunc, U currFunc, ReturnType (*currentFunc2)() )
	{
		U actualFunc = 
			reinterpret_cast< U >( foundFunc );
		return actualFunc();
	}


	static SingletonManager & instance() { return *s_pInstance; }

private:
	typedef std::unordered_map< std::string, InstanceFunc > CollectionType;
	CollectionType s_Singletons;
	static std::auto_ptr< SingletonManager > s_pInstance;
};

}

#endif //SINGLETON_MANAGER_HPP

