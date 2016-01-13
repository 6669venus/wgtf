#ifndef REFLECTED_OBJECT_HPP
#define REFLECTED_OBJECT_HPP

#include <cassert>
#include <memory>
#include <type_traits>
#include <typeinfo>

class IClassDefinition;

template< typename T >
class TypeClassDefinition;

template< typename T >
const char * getClassIdentifier()
{
	return typeid( T ).name();
}

#define DECLARE_REFLECTED \
	template< typename T >\
	friend class TypeClassDefinition;\

class ReflectedPolyStruct 
{
public:
	ReflectedPolyStruct()
		: definition_( nullptr )
	{
	}

	virtual ~ReflectedPolyStruct() { }

	virtual const IClassDefinition & getDefinition() const
	{
		assert( definition_ != nullptr );
		return *definition_;
	}

	void setDefinition( const IClassDefinition * definition )
	{
		definition_ = definition;
	}

private:
	const IClassDefinition * definition_;
};


#endif // REFLECTED_OBJECT_HPP
