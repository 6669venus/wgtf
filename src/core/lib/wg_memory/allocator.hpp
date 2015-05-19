#ifndef NGT_ALLOCATOR_HPP
#define NGT_ALLOCATOR_HPP

namespace NGTAllocator
{
	void init();
	void * allocate( size_t size );
	void deallocate( void* ptr );
	void * createMemoryContext( const wchar_t * name );
	void destroyMemoryContext( void * );
	void pushMemoryContext( void * );
	void popMemoryContext();
	void cleanupContext( void * );
}

#endif // NGT_ALLOCATOR_HPP
