#include "default_meta_type_manager.hpp"
#include "wg_types/binary_block.hpp"
#include "meta_type.hpp"
#include "variant.hpp"
#include "collection.hpp"

#include <iostream>
#include <unordered_map>

namespace
{

	template<typename T>
	void genericInit(void* value)
	{
		new (value) T();
	}


	template<typename T>
	void genericDestroy(void* value)
	{
		reinterpret_cast<T*>(value)->~T();
	}


	class VoidMetaType
		: public MetaType
	{
		typedef MetaType base;

	public:
		VoidMetaType():
			base(typeid(void), static_cast<size_t>(0), nullptr)
		{
		}

		void init(void* value) const override
		{
			// nop
		}

		void copy(void* dest, const void* src) const override
		{
			// nop
		}

		void move(void* dest, void* src) const override
		{
			// nop
		}

		void destroy(void* value) const override
		{
			// nop
		}

		bool equal(const void* lhs, const void* rhs) const override
		{
			return true;
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			stream << "void";
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}

			std::string str;
			std::getline( stream, str );
			return str == "void";
		}

	};


	class StringMetaType
		: public MetaType
	{
		typedef MetaType base;

	public:
		StringMetaType():
			base(typeid(std::string), sizeof(std::string), nullptr, ForceShared)
		{
		}

		void init(void* value) const override
		{
			genericInit<std::string>(value);
		}

		void copy(void* dest, const void* src) const override
		{
			*cast(dest) = *cast(src);
		}

		void move(void* dest, void* src) const override
		{
			*cast(dest) = std::move(*cast(src));
		}

		void destroy(void* value) const override
		{
			genericDestroy<std::string>(value);
		}

		bool equal(const void* lhs, const void* rhs) const override
		{
			return *cast(lhs) == *cast(rhs);
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			return Variant::streamOut(stream, *cast(value));
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			return Variant::streamIn(stream, *cast(value));
		}

	private:
		static std::string* cast(void* value)
		{
			return static_cast<std::string*>(value);
		}

		static const std::string* cast(const void* value)
		{
			return static_cast<const std::string*>(value);
		}

	};

}

//==============================================================================
DefaultMetaTypeManager::DefaultMetaTypeManager()
	: typeNameToMetaType_()
	, typeInfoToMetaType_()
{
	defaultMetaTypes_.emplace_back( new VoidMetaType() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< void* >() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< const void * >() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< uint64_t >() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< int64_t >() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< float >() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< double>() );
	defaultMetaTypes_.emplace_back( new StringMetaType );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Collection >() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< std::shared_ptr< BinaryBlock > >() );

	for( auto it = defaultMetaTypes_.begin(); it != defaultMetaTypes_.end(); ++it )
	{
		registerType( it->get() );
	}
}

//==============================================================================
bool DefaultMetaTypeManager::registerType(const MetaType* type)
{
	bool nameOk = typeNameToMetaType_.insert(TypeNameToMetaType::value_type(type->name(), type)).second;
	bool typeInfoOk = typeInfoToMetaType_.insert(TypeInfoToMetaType::value_type(&type->typeInfo(), type)).second;
	return nameOk && typeInfoOk;
}


//==============================================================================
const MetaType* DefaultMetaTypeManager::findType(const char* name ) const
{
	TypeNameToMetaType::const_iterator it = typeNameToMetaType_.find(name);
	if(it != typeNameToMetaType_.end())
	{
		return it->second;
	}

	return nullptr;
}


//==============================================================================
const MetaType* DefaultMetaTypeManager::findType(const type_info& typeInfo)  const
{
	TypeInfoToMetaType::const_iterator it = typeInfoToMetaType_.find(&typeInfo);
	if(it != typeInfoToMetaType_.end())
	{
		return it->second;
	}

	return nullptr;
}


//==============================================================================
void DefaultMetaTypeManager::registerDynamicStorageHandler(
	const IStorageLookupHandler & handler )
{
	storageHandlers_.insert( &handler );
}


//==============================================================================
void DefaultMetaTypeManager::deregisterDynamicStorageHandler(
	const IStorageLookupHandler & handler )
{
	storageHandlers_.erase( &handler );
}


//==============================================================================
const IStorageLookupHandler * DefaultMetaTypeManager::dynamicStorageHandlerLookup(
	const TypeId & typeId ) const
{
	for( auto storageHandler : storageHandlers_ )
	{
		auto metaType = storageHandler->findStorageType( typeId );
		if (metaType != nullptr)
		{
			return storageHandler;
		}
	}
	return nullptr;
}
