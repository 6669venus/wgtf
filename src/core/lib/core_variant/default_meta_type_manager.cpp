#include "default_meta_type_manager.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
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

	class BinaryBlockSharedPtrMetaType
		: public MetaTypeImpl<std::shared_ptr< BinaryBlock >>
	{
		typedef MetaTypeImpl<std::shared_ptr< BinaryBlock >> base;

	public:
		BinaryBlockSharedPtrMetaType():
			base( nullptr, ForceShared )
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const auto & binary = *cast(value);
			std::string tmp( binary->cdata(), binary->length() );
			return Variant::streamOut( stream, tmp );
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if(!stream.good())
			{
				return false;
			}
			std::string tmp;
			bool br = Variant::streamIn( stream, tmp );
			if (!br)
			{
				return false;
			}
			auto & binary = *cast(value);
			binary = std::make_shared< BinaryBlock >( tmp.c_str(), tmp.length(), false );
			return br;
		}

	private:
		static std::shared_ptr< BinaryBlock >* cast(void* value)
		{
			return static_cast<std::shared_ptr< BinaryBlock >*>(value);
		}

		static const std::shared_ptr< BinaryBlock >* cast(const void* value)
		{
			return static_cast<const std::shared_ptr< BinaryBlock >*>(value);
		}

	};

	const char g_separator = ',';

	class Vector3MetaType
		: public MetaTypeImpl<Vector3>
	{
		typedef MetaTypeImpl<Vector3> base;

	public:
		Vector3MetaType() :
			base(nullptr, ForceShared)
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const Vector3 & vec = *cast(value);
			stream << vec.x << g_separator << vec.y << g_separator << vec.z;
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}
			Vector3 & vec = *cast(value);
			char separator;
			stream >> vec.x >> separator >> vec.y >> separator >> vec.z;
			return !stream.fail();
		}

	private:
		static Vector3* cast(void* value)
		{
			return static_cast<Vector3*>(value);
		}

		static const Vector3 * cast(const void* value)
		{
			return static_cast<const Vector3*>(value);
		}
	};

	class Vector4MetaType
		: public MetaTypeImpl<Vector4>
	{
		typedef MetaTypeImpl<Vector4> base;

	public:
		Vector4MetaType() :
			base(nullptr, ForceShared)
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const Vector4 & vec = *cast(value);
			stream << vec.x << g_separator << vec.y << g_separator << vec.z << g_separator << vec.w;
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}
			Vector4 & vec = *cast(value);
			char separator;
			stream >> vec.x >> separator >> vec.y >> separator >> vec.z >> separator >> vec.w;
			return !stream.fail();
		}

	private:
		static Vector4* cast(void* value)
		{
			return static_cast<Vector4*>(value);
		}

		static const Vector4 * cast(const void* value)
		{
			return static_cast<const Vector4*>(value);
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
	defaultMetaTypes_.emplace_back( new BinaryBlockSharedPtrMetaType() );
	defaultMetaTypes_.emplace_back( new Vector3MetaType() );
	defaultMetaTypes_.emplace_back( new Vector4MetaType() );

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
