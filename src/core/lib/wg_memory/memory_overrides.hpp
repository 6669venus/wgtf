#ifndef BW_MEMORY_OPERATIONS_HPP
#define BW_MEMORY_OPERATIONS_HPP

#include <cstddef>
#include <new>
#include "allocator.hpp"

void * operator new( std::size_t size )										
{																			
	return NGTAllocator::allocate( size );
}																			

void * operator new ( std::size_t size, const std::nothrow_t& )				
{																			
	return NGTAllocator::allocate( size );
}																			

void * operator new[]( std::size_t size )									
{																			
	return NGTAllocator::allocate( size );
}																			

void * operator new[]( std::size_t size, const std::nothrow_t & throwable )	
{																			
	return NGTAllocator::allocate( size );
}																			

void operator delete( void* ptr )											
{																			
	NGTAllocator::deallocate( ptr );
}																			

void operator delete( void* ptr, const std::nothrow_t & throwable )			
{																			
	NGTAllocator::deallocate( ptr );
}																			

void operator delete[]( void* ptr )											
{																			
	NGTAllocator::deallocate( ptr );
}																			

void operator delete[]( void* ptr, const std::nothrow_t & throwable )		
{																			
	NGTAllocator::deallocate( ptr );
}

#endif