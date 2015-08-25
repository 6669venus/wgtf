#ifndef I_META_TYPE_MANAGER_HPP
#define I_META_TYPE_MANAGER_HPP

#include <typeinfo>
#include "core_dependency_system/i_interface.hpp"

class MetaType;

class IStorageLookupHandler
{
public:
	virtual ~IStorageLookupHandler() {}
	virtual const MetaType * findStorageType( const TypeId & typeId ) const = 0;
	virtual size_t storageSize() const = 0;
	virtual void initStorage( void * pStorage, const TypeId & typeId,
		const void * value ) const = 0;
	//Throws the return type as there is no way to implement a templated
	//virtual function
	virtual void tryConvert(
		const TypeId & sourceType, const void * pSource ) const = 0;
};


class IMetaTypeManager
{
public:
	/**
	Register user type.
	Use this function to add support of any type.

	MetaType ownership is not transferred to Variant, so you're responsible
	for its life time management. You must ensure that MetaType is valid
	while at least one instance of Variant with value of corresponding type
	exist. Undefined behavior (most likely crash) may occur otherwise.

	Returns whether type was successfully registered. Registration may fail when
	the type with the same name and/or matching @c type_info is already
	registered.

	@see typeIsRegistered, findType
	*/
	virtual bool registerType(const MetaType* type) = 0;

	template< typename T >
	const MetaType* findType() const
	{
		return findType( typeid( T ) );
	}

	/**
	Find registered MetaType by name.

	@see registerType
	*/
	virtual const MetaType* findType(const char* name) const = 0;

	/**
	Find registered MetaType by @c type_info.

	@see registerType
	*/
	virtual const MetaType* findType(const std::type_info& typeInfo) const = 0;


	virtual void registerDynamicStorageHandler(
		const IStorageLookupHandler & handler ) = 0;
	virtual void deregisterDynamicStorageHandler(
		const IStorageLookupHandler & handler ) = 0;

	virtual const IStorageLookupHandler * dynamicStorageHandlerLookup(
		const TypeId & typeId ) const = 0;
};

#endif // I_META_TYPE_MANAGER_HPP
