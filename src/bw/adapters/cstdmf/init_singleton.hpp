#ifndef INIT_SINGLETON_HPP
#define INIT_SINGLETON_HPP

#include "singleton.hpp"

namespace BW
{

template <class T>
class InitSingleton : public Singleton<T>
{
public:
	static bool init()
	{
		assert( !s_finalised_ );

		bool result = true;

		if ( s_initedCount_ == 0 )
		{
			InitSingleton<T>* baseInstance = new T();
			result = baseInstance->doInit();
		}

		++s_initedCount_;
		return result;
	}

	static bool fini()
	{
		assert( !s_finalised_ );
		assert( s_initedCount_ > 0 );

		--s_initedCount_;

		bool result = true;

		if ( s_initedCount_ == 0 )
		{
			InitSingleton<T>* baseInstance = Singleton<T>::pInstance();
			result = baseInstance->doFini();
			delete Singleton<T>::pInstance();

			s_finalised_ = true;
		}

		return result;
	}

	static bool finalised() { return s_finalised_; }

protected:
	static bool s_finalised_;
	static int s_initedCount_;

	virtual ~InitSingleton() {}
	virtual bool doInit() { return true; }
	virtual bool doFini() { return true; }
};

}

#endif // INIT_SINGLETON_HPP
