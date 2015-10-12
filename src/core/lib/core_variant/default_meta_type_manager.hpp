#ifndef DEFAULT_META_TYPE_MANAGER_HPP
#define DEFAULT_META_TYPE_MANAGER_HPP

#include <memory>

#include "interfaces/i_meta_type_manager.hpp"
#include "wg_types/hash_utilities.hpp"
#include <set>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <cstring>

class DefaultMetaTypeManager
	: public Implements< IMetaTypeManager >
{
public:
	DefaultMetaTypeManager();
	
	bool registerType(const MetaType* type) override;
	const MetaType* findType(const char* name) const override;
	const MetaType* findType(const std::type_info& typeInfo) const override;

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
				std::strcmp(lhs, rhs) == 0;
		}
	};


	struct TypeInfoHash
	{
		size_t operator()(const std::type_info* v) const
		{
			return static_cast<size_t>( HashUtilities::compute(v->name()) );
		}
	};


	struct TypeInfosEq
	{
		bool operator()(const std::type_info* lhs, const std::type_info* rhs) const
		{
			return lhs->name() == rhs->name() || !strcmp(lhs->name(), rhs->name());
		}
	};

	typedef std::unordered_map<const char*, const MetaType*, NameHash, NamesEq> TypeNameToMetaType;
	typedef std::unordered_map<const std::type_info*, const MetaType*, TypeInfoHash, TypeInfosEq> TypeInfoToMetaType;

	TypeNameToMetaType typeNameToMetaType_;
	TypeInfoToMetaType typeInfoToMetaType_;
	std::vector< std::unique_ptr< MetaType > > defaultMetaTypes_;
};

#endif //DEFAULT_META_TYPE_MANAGER_HPP
