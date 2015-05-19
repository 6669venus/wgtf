#ifndef PROPERTY_ITERATOR_HPP
#define PROPERTY_ITERATOR_HPP

#include <vector>

class IClassDefinition;
class IBaseProperty;

typedef std::vector< IBaseProperty * > SortedPropertyCollection;

class PropertyIterator
{
public:
	enum IterateStrategy
	{
		ITERATE_PARENTS,
		ITERATE_SELF_ONLY
	};

	PropertyIterator();
	PropertyIterator( const IClassDefinition * definition, IterateStrategy strategy);

	IBaseProperty * current() const;
	void next();

	IBaseProperty * operator*();
	IBaseProperty * operator->();

	PropertyIterator & operator++();

	bool operator==(const PropertyIterator& other) const;
	bool operator!=(const PropertyIterator& other) const;

private:
	const IClassDefinition *					definition_;
	const SortedPropertyCollection *			collection_;
	SortedPropertyCollection::const_iterator	currentIt_;
	IterateStrategy								strategy_;
};


// Helper class for use with range-based-for loops and STL algorithms
// Allows std::begin() and std::end() to work for PropertyIterators.
class PropertyIteratorRange
{
public:
	PropertyIteratorRange(const IClassDefinition * definition, 
		PropertyIterator::IterateStrategy strategy);

	PropertyIterator begin() const;
	PropertyIterator end() const;

private:
	const IClassDefinition * definition_;
	PropertyIterator::IterateStrategy strategy_;
};

#endif // PROPERTY_ITERATOR_HPP
