#ifndef PROPERTY_STORAGE_HPP
#define PROPERTY_STORAGE_HPP

#include "property_iterator.hpp"

#include <vector>
#include <memory>

class IBaseProperty;

typedef std::vector< IBasePropertyPtr > SortedProperties;

class PropertyStorage
{
public:
	PropertyStorage();
	~PropertyStorage();

	void addProperty( const IBasePropertyPtr & property );

private:
	SortedProperties properties_;

	friend class PropertyStorageIterator;
};

class PropertyStorageIterator : public PropertyIteratorImplBase
{
public:
	PropertyStorageIterator( const PropertyStorage & storage );

	virtual std::shared_ptr< IBaseProperty > current() const override;

	virtual bool next() override;

private:
	IBasePropertyPtr current_;
	SortedProperties::const_iterator iterator_;
	SortedProperties::const_iterator end_;
};

#endif