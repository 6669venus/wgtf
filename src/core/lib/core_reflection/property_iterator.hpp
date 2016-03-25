#ifndef PROPERTY_ITERATOR_HPP
#define PROPERTY_ITERATOR_HPP

#include <vector>
#include <memory>

#include "reflection_dll.hpp"

class IClassDefinition;
class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;
class PropertyIteratorImplBase;
typedef std::shared_ptr< PropertyIteratorImplBase > PropertyIteratorImplPtr;

class REFLECTION_DLL PropertyIteratorImplBase
{
public:
	virtual ~PropertyIteratorImplBase() {}

	virtual IBasePropertyPtr current() const = 0;
	virtual bool next() = 0;
};

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

class REFLECTION_DLL PropertyIterator
{
public:
	enum IterateStrategy
	{
		ITERATE_PARENTS,
		ITERATE_SELF_ONLY
	};

	PropertyIterator();
	PropertyIterator( IterateStrategy strategy, const IClassDefinition & definition );

	IBasePropertyPtr get() const;

	IBasePropertyPtr operator*() const;
	IBasePropertyPtr operator->() const;

	PropertyIterator & operator++();

	bool operator==(const PropertyIterator& other) const;
	bool operator!=(const PropertyIterator& other) const;

private:
	void moveNext();

	IterateStrategy								strategy_;
	const IClassDefinition *					currentDefinition_;
	PropertyIteratorImplPtr						currentIterator_;
};

#pragma warning (pop)

// Helper class for use with range-based-for loops and STL algorithms
// Allows std::begin() and std::end() to work for PropertyIterators.
class REFLECTION_DLL PropertyIteratorRange
{
public:
	PropertyIteratorRange( PropertyIterator::IterateStrategy strategy, const IClassDefinition & definition );

	PropertyIterator begin() const;
	PropertyIterator end() const;

private:
	PropertyIterator::IterateStrategy strategy_;
	const IClassDefinition & definition_;
};

#endif // PROPERTY_ITERATOR_HPP
