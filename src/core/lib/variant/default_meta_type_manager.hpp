#ifndef DEFAULT_META_TYPE_MANAGER_HPP
#define DEFAULT_META_TYPE_MANAGER_HPP

#include <memory>

#include "interfaces/i_meta_type_manager.hpp"
#include "wg_types/hash_utilities.hpp"
#include <set>
#include <vector>
#include <unordered_map>

class DefaultMetaTypeManager
	: public Implements< IMetaTypeManager >
{
public:
	DefaultMetaTypeManager();

	bool registerType(const MetaType* type) override;
	const MetaType* findType(const char* name) const override;
	const MetaType* findType(const type_info& typeInfo) const override;

	void registerDynamicStorageHandler(
		const IStorageLookupHandler & handler ) override;
	void deregisterDynamicStorageHandler(
		const IStorageLookupHandler & handler ) override;
	const IStorageLookupHandler * dynamicStorageHandlerLookup(
		const TypeId & typeId ) const override;

private:
	struct NameHash
	{
		size_t operator()(const char* v) const
		{
			return static_cast<size_t>( HashUtilities::compute( v ) );
		}
	};


	struct NamesEq
	{
		bool operator()(const char* lhs, const char* rhs) const
		{
			return
				lhs == rhs ||
				strcmp(lhs, rhs) == 0;
		}
	};


	struct TypeInfoHash
	{
		size_t operator()(const type_info* v) const
		{
			return v->hash_code();
		}
	};


	struct TypeInfosEq
	{
		bool operator()(const type_info* lhs, const type_info* rhs) const
		{
			return
				lhs == rhs ||
				*lhs == *rhs;
		}
	};

	typedef std::unordered_map<const char*, const MetaType*, NameHash, NamesEq> TypeNameToMetaType;
	typedef std::unordered_map<const type_info*, const MetaType*, TypeInfoHash, TypeInfosEq> TypeInfoToMetaType;

	TypeNameToMetaType typeNameToMetaType_;
	TypeInfoToMetaType typeInfoToMetaType_;
	std::vector< std::unique_ptr< MetaType > > defaultMetaTypes_;
	std::set< const IStorageLookupHandler * > storageHandlers_;
};

#endif //DEFAULT_META_TYPE_MANAGER_HPP