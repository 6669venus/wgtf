#ifndef QT_NEW_HANDLER
#define QT_NEW_HANDLER

#include <set>

std::set< void * > & getQtInPlaceNewCollection();
void releaseQtInPlaceNewCollection();

#define DECLARE_QT_MEMORY_HANDLER \
public:\
void *	operator new( size_t s )\
{\
	return ::operator new( s );\
}\
\
void operator delete( void* ptr )\
{\
	auto findIt = getQtInPlaceNewCollection().find( ptr );\
	if (findIt != getQtInPlaceNewCollection().end())\
	{\
		getQtInPlaceNewCollection().erase( findIt );\
		if (getQtInPlaceNewCollection().empty())\
		{\
			releaseQtInPlaceNewCollection();\
		}\
		return;\
	}\
	::operator delete( ptr );\
}\
\
void *	operator new( size_t s, void * at )\
{\
	getQtInPlaceNewCollection().insert( at );\
	return at;\
}\
\
void operator delete( void* p, void * )\
{\
	getQtInPlaceNewCollection().erase( p );\
	if (getQtInPlaceNewCollection().empty())\
	{\
		releaseQtInPlaceNewCollection();\
	}\
};\

#endif //QT_NEW_HANDLER