#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include "singleton_manager.hpp"
#include <assert.h>

namespace BW
{

template <class T>
class Singleton
{
protected:
	static T * s_pInstance;

public:
	Singleton()
	{
		assert( NULL == s_pInstance );
		s_pInstance = static_cast< T * >( this );
		REGISTER_SINGLETON_FUNC( T, &T::pInstance );
	}

	~Singleton()
	{
		assert( this == s_pInstance );
		s_pInstance = NULL;
		DEREGISTER_SINGLETON_FUNC( T );
	}

	static T & instance()
	{
		T * instanceP = pInstance();
		assert( instanceP );
		return *instanceP;
	}

	static T * pInstance()
	{
		SINGLETON_MANAGER_WRAPPER_FUNC( T, &T::pInstance )
		return s_pInstance;
	}
};

#if defined (__INTEL_COMPILER)
#define BW_SINGLETON_STORAGE( TYPE )						\
	TYPE * BW::Singleton< TYPE >::s_pInstance = NULL;
#else
#define BW_SINGLETON_STORAGE( TYPE )						\
	template <>													\
	TYPE * BW::Singleton< TYPE >::s_pInstance = NULL;
#endif

}

#endif // SINGLETON_HPP
