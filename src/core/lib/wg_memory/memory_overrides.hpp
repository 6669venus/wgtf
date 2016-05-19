#ifndef BW_MEMORY_OPERATIONS_HPP
#define BW_MEMORY_OPERATIONS_HPP

#include <cstddef>
#include <new>
#include "allocator.hpp"

#ifdef __APPLE__
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif // __APPLE__

#ifdef NGT_ALLOCATOR

namespace wgt
{
void * operator new( std::size_t size )
{																			
	return NGTAllocator::allocate( size );
}																			

void * operator new ( std::size_t size, const std::nothrow_t& )	NOEXCEPT
{																			
	return NGTAllocator::allocate( size );
}																			

void * operator new[]( std::size_t size )									
{																			
	return NGTAllocator::allocate( size );
}																			

void * operator new[]( std::size_t size, const std::nothrow_t & throwable )	NOEXCEPT
{																			
	return NGTAllocator::allocate( size );
}																			

void operator delete( void* ptr ) NOEXCEPT
{																			
	NGTAllocator::deallocate( ptr );
}																			

void operator delete( void* ptr, const std::nothrow_t & throwable )	NOEXCEPT
{																			
	NGTAllocator::deallocate( ptr );
}																			

void operator delete[]( void* ptr ) NOEXCEPT
{																			
	NGTAllocator::deallocate( ptr );
}																			

void operator delete[]( void* ptr, const std::nothrow_t & throwable ) NOEXCEPT
{																			
	NGTAllocator::deallocate( ptr );
}

#endif //NGT_ALLOCATOR

} // end namespace wgt
#endif // BW_MEMORY_OPERATIONS_HPP
