#ifndef PROPERTY_STORAGE_HPP
#define PROPERTY_STORAGE_HPP

#include <vector>
#include <memory>

namespace wgt
{
class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;
typedef std::vector< IBasePropertyPtr > SortedProperties;
class PropertyIteratorImplBase;
typedef std::shared_ptr< PropertyIteratorImplBase > PropertyIteratorImplPtr;

class PropertyStorage
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

}
#endif
