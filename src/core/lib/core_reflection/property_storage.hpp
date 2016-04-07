#ifndef PROPERTY_STORAGE_HPP
#define PROPERTY_STORAGE_HPP

#include <vector>
#include <memory>

#include "reflection_dll.hpp"

class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;
typedef std::vector< IBasePropertyPtr > SortedProperties;
class PropertyIteratorImplBase;
typedef std::shared_ptr< PropertyIteratorImplBase > PropertyIteratorImplPtr;

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

class REFLECTION_DLL PropertyStorage
{
public:
	PropertyStorage();
	~PropertyStorage();

	void addProperty( const IBasePropertyPtr & property );
	PropertyIteratorImplPtr getIterator() const;

private:
	SortedProperties properties_;

	friend class PropertyStorageIterator;
};

#pragma warning (pop)

#endif