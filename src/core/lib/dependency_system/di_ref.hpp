#ifndef DI_REF_HPP
#define DI_REF_HPP

#include <vector>
#include "dependency_system/i_interface.hpp"

template< class T >
class DIRef
{
public:
	//==========================================================================
	T * get() const
	{
		//TODO: Cache the data and dirty when new interfaces are added / removed
		return Context::queryInterface< T >();
	}


	//==========================================================================
	T * operator->() const
	{
		return get();
	}


	//==========================================================================
	T & operator*() const
	{
		assert( get() != nullptr );
		return *get();
	}
};

template< class T >
class DIRefMany
{
public:
	std::vector< T * > get() const
	{
		//TODO: Cache the data and dirty when new interfaces are added / removed
		std::vector< T * > refs;
		Context::queryInterface< T >( refs );
		return refs;
	}
};

#endif //DI_REF_HPP