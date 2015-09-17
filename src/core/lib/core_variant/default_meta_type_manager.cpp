#include "default_meta_type_manager.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "meta_type.hpp"
#include "variant.hpp"
#include "collection.hpp"

#include <iostream>

namespace
{

	class VoidMetaType
		: public MetaType
	{
		typedef MetaType base;

	public:
		VoidMetaType():
			base( "void", 0, typeid( void ), nullptr, 0 )
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

		void streamOut(TextStream& stream, const void* value) const override
		{
			stream << "void";
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			stream >> TextPatternChecker( "void" );
		}

		void streamOut(BinaryStream& stream, const void* value) const override
		{
			// nop
		}

		void streamIn(BinaryStream& stream, void* value) const override
		{
			// nop
		}
	};


	class StringMetaType
		: public MetaTypeImplNoStream<std::string>
	{
		typedef MetaTypeImplNoStream<std::string> base;

	public:
		StringMetaType():
			base( nullptr, ForceShared )
		{
		}

		void streamOut(TextStream& stream, const void* value) const override
		{
			stream << quoted( base::cast(value) );
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			stream >> quoted( base::cast(value) );
		}

		void streamOut(BinaryStream& stream, const void* value) const override
		{
			stream << base::cast(value);
		}

		void streamIn(BinaryStream& stream, void* value) const override
		{
			stream >> base::cast(value);
		}
	};

	class BinaryBlockSharedPtrMetaType
		: public MetaTypeImplNoStream<std::shared_ptr< BinaryBlock >>
	{
		typedef MetaTypeImplNoStream<std::shared_ptr< BinaryBlock >> base;

	public:
		BinaryBlockSharedPtrMetaType():
			base( nullptr, 0 )
		{
		}

		void streamOut(TextStream& stream, const void* value) const override
		{
			const auto& binary = base::cast(value);
			FixedMemoryStream dataStream( binary->cdata(), binary->length() );
			stream.serializeString( dataStream ); // TODO: serialize to Base64?
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			ResizingMemoryStream dataStream;
			stream.deserializeString( dataStream );
			if (!stream.fail())
			{
				base::cast(value) = std::make_shared< BinaryBlock >(
					dataStream.buffer().c_str(),
					dataStream.buffer().length(),
					false );
			}
		}

		void streamOut(BinaryStream& stream, const void* value) const override
		{
			const auto& binary = base::cast(value);
			stream.serializeBuffer( binary->cdata(), binary->length() );
		}

		void streamIn(BinaryStream& stream, void* value) const override
		{
			ResizingMemoryStream dataStream;
			stream.deserializeBuffer( dataStream );
			if (!stream.fail())
			{
				base::cast(value) = std::make_shared< BinaryBlock >(
					dataStream.buffer().c_str(),
					dataStream.buffer().length(),
					false );
			}
		}
	};

	const char g_separator = ',';

	// Vector2

	TextStream& operator<<( TextStream& stream, const Vector2& v )
	{
		stream << v.x << g_separator << v.y;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, Vector2& v )
	{
		TextPatternChecker separator( g_separator );
		stream >> v.x >> separator >> v.y;
		return stream;
	}

	BinaryStream& operator<<( BinaryStream& stream, const Vector2& v)
	{
		stream << v.x << v.y;
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, Vector2& v)
	{
		stream >> v.x >> v.y;
		return stream;
	}

	// Vector3

	TextStream& operator<<( TextStream& stream, const Vector3& v )
	{
		stream << v.x << g_separator << v.y << g_separator << v.z;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, Vector3& v )
	{
		TextPatternChecker separator( g_separator );
		stream >> v.x >> separator >> v.y >> separator >> v.z;
		return stream;
	}

	BinaryStream& operator<<( BinaryStream& stream, const Vector3& v)
	{
		stream << v.x << v.y << v.z;
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, Vector3& v)
	{
		stream >> v.x >> v.y >> v.z;
		return stream;
	}

	// Vector4

	TextStream& operator<<( TextStream& stream, const Vector4& v )
	{
		stream << v.x << g_separator << v.y << g_separator << v.z << g_separator << v.w;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, Vector4& v )
	{
		TextPatternChecker separator( g_separator );
		stream >> v.x >> separator >> v.y >> separator >> v.z >> separator >> v.w;
		return stream;
	}

	BinaryStream& operator<<( BinaryStream& stream, const Vector4& v)
	{
		stream << v.x << v.y << v.z << v.w;
		return stream;
	}

	BinaryStream& operator>>( BinaryStream& stream, Vector4& v)
	{
		stream >> v.x >> v.y >> v.z >> v.w;
		return stream;
	}

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
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Vector2 >( "vector2" ) );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Vector3 >( "vector3" ) );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Vector4 >( "vector4" ) );

	for( auto it = defaultMetaTypes_.begin(); it != defaultMetaTypes_.end(); ++it )
	{
		registerType( it->get() );
	}
}

//==============================================================================
bool DefaultMetaTypeManager::registerType(const MetaType* type)
{
	bool nameOk = typeNameToMetaType_.emplace(type->name(), type).second;
	bool typeInfoOk = typeInfoToMetaType_.emplace(type->typeId(), type).second;
	return nameOk && typeInfoOk;
}


//==============================================================================
const MetaType* DefaultMetaTypeManager::findType(const char* name ) const
{
	auto it = typeNameToMetaType_.find(name);
	if(it != typeNameToMetaType_.end())
	{
		return it->second;
	}

	return nullptr;
}


//==============================================================================
const MetaType* DefaultMetaTypeManager::findType(const TypeId& typeId)  const
{
	auto it = typeInfoToMetaType_.find(typeId);
	if(it != typeInfoToMetaType_.end())
	{
		return it->second;
	}

	return nullptr;
}

