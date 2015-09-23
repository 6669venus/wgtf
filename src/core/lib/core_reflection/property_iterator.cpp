#include "property_iterator.hpp"
#include "interfaces/i_class_definition.hpp"
#include "interfaces/i_base_property.hpp"

// =============================================================================
PropertyIterator::PropertyIterator()
	: definition_( nullptr )
	, collection_( nullptr )
	, strategy_(ITERATE_SELF_ONLY)
{
}

// =============================================================================
PropertyIterator::PropertyIterator(
		const IClassDefinition * definition, IterateStrategy strategy )
	: definition_( definition )
	, collection_( nullptr )
	, strategy_( strategy )
{
	if (definition_ != NULL)
	{
		collection_ = &definition_->sortedProperties();
		currentIt_ = collection_->begin();
		if (currentIt_ == collection_->end())
		{
			next();
		}
	}
}


// =============================================================================
IBaseProperty * PropertyIterator::current() const
{
	if (collection_ == nullptr ||
		collection_->empty() ||
		currentIt_ == collection_->end())
	{
		return nullptr;
	}
	return *currentIt_;
}


// =============================================================================
void PropertyIterator::next()
{
	// get next property in chain
	if (currentIt_ != collection_->end())
	{
		currentIt_++;
		if (currentIt_ != collection_->end())
		{
			return;
		}
	}

	// get parent's properties
	if (definition_ != NULL)
	{
		if (strategy_ == ITERATE_PARENTS)
		{
			while (current() == nullptr)
			{
				definition_ = definition_->getParent();
				if (definition_ == NULL)
				{
					break;
				}
				collection_ = &definition_->sortedProperties();
				currentIt_ = collection_->begin();
			}
		}
		else
		{
			// set definition to NULL to match default constructed iterator
			definition_ = NULL;
		}
	}
}


IBaseProperty * PropertyIterator::operator*()
{
	return current();
}

IBaseProperty * PropertyIterator::operator->()
{
	return current();
}

PropertyIterator & PropertyIterator::operator++()
{
	next();
	return *this;
}

bool PropertyIterator::operator==(const PropertyIterator& other) const
{
	return definition_ == other.definition_ && current() == other.current();
}

bool PropertyIterator::operator!=(const PropertyIterator& other) const
{
	return !operator==( other );
}


PropertyIteratorRange::PropertyIteratorRange(
	const IClassDefinition * definition, PropertyIterator::IterateStrategy strategy)
	: definition_(definition), strategy_(strategy)
{
}

PropertyIterator PropertyIteratorRange::begin() const
{
	return PropertyIterator(definition_, strategy_);
}

PropertyIterator PropertyIteratorRange::end() const
{
	return PropertyIterator();
}