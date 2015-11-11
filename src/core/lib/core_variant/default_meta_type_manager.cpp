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

#include <typeinfo>
#include <cstdint>
#include <string>
#include <memory>


namespace
{

	class VoidMetaType
		: public MetaType
	{
		typedef MetaType base;

		static bool convertToVoid( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
		{
			return true;
		}

	public:
		VoidMetaType():
			base(
				"void",
				0,
				TypeId::getType< void >(),
				typeid( void ),
				nullptr,
				DeducibleFromText )
		{
			setDefaultConversionFrom( &convertToVoid );
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

#if !FAST_RUNTIME_POINTER_CAST

		void throwPtr( void* ptr, bool const_value ) const override
		{
			// nop
		}

#endif

	};


	class PtrMetaType:
		public MetaTypeImpl< void* >
	{
		typedef MetaTypeImpl< void* > base;

		static bool convertToVoidPtr( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
		{
			if( auto ptr = fromType->castPtr< void* >( from ) )
			{
				toType->copy( to, &ptr );
				return true;
			}

			return false;
		}

	public:
		PtrMetaType():
			base( "ptr" )
		{
			setDefaultConversionFrom( &convertToVoidPtr );
		}

	};


	class ConstPtrMetaType:
		public MetaTypeImpl< const void* >
	{
		typedef MetaTypeImpl< const void* > base;

		static bool convertToConstVoidPtr( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
		{
			if( auto ptr = fromType->castPtr< const void* >( from ) )
			{
				toType->copy( to, &ptr );
				return true;
			}

			return false;
		}

	public:
		ConstPtrMetaType():
			base( "ptr" )
		{
			setDefaultConversionFrom( &convertToConstVoidPtr );
		}

	};


	class UIntMetaType:
		public MetaTypeImpl< uintmax_t >
	{
		typedef MetaTypeImpl< uintmax_t > base;

	public:
		UIntMetaType():
			base( "uint", DeducibleFromText )
		{
			addStraightConversion< uintmax_t, intmax_t >();
			addStraightConversion< uintmax_t, double >();
		}
	};


	class IntMetaType:
		public MetaTypeImpl< intmax_t >
	{
		typedef MetaTypeImpl< intmax_t > base;

	public:
		IntMetaType():
			base( "int", DeducibleFromText )
		{
			addStraightConversion< intmax_t, uintmax_t >();
			addStraightConversion< intmax_t, double >();
		}
	};


	class RealMetaType:
		public MetaTypeImpl< double >
	{
		typedef MetaTypeImpl< double > base;

	public:
		RealMetaType():
			base( "real", DeducibleFromText )
		{
			addStraightConversion< double, uintmax_t >();
			addStraightConversion< double, intmax_t >();
		}
	};


	class Vector2MetaType
		: public MetaTypeImpl<Vector2>
	{
		typedef MetaTypeImpl<Vector2> base;

	public:
		Vector2MetaType() :
			base(nullptr, ForceShared)
		{
		}

		bool streamOut(std::ostream& stream, const void* value) const override
		{
			const Vector2 & vec = *cast(value);
			stream << vec.x << g_separator << vec.y;
			return stream.good();
		}

		bool streamIn(std::istream& stream, void* value) const override
		{
			if (!stream.good())
			{
				return false;
			}
			Vector2 & vec = *cast(value);
			char separator;
			stream >> vec.x >> separator >> vec.y;
			return !stream.fail();
		}

	private:
		static Vector2* cast(void* value)
		{
			return static_cast<Vector2*>(value);
		}

		static const Vector2 * cast(const void* value)
		{
			return static_cast<const Vector2*>(value);
		}
	};

	class StringMetaType
		: public MetaTypeImplNoStream<std::string>
	{
		typedef MetaTypeImplNoStream<std::string> base;

		static bool convertToString( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
		{
			ResizingMemoryStream dataStream;
			TextStream stream( dataStream );
			fromType->streamOut( stream, from );
			if( stream.fail() )
			{
				return false;
			}

			stream.sync();
			std::string& toStr = *reinterpret_cast<std::string*>( to );
			toStr = dataStream.takeBuffer();
			return true;
		}

	public:
		StringMetaType():
			base( "string", ForceShared | DeducibleFromText )
		{
			setDefaultConversionFrom( &convertToString );
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
			base( "blob", 0 )
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
	defaultMetaTypes_.emplace_back( new PtrMetaType() );
	defaultMetaTypes_.emplace_back( new ConstPtrMetaType() );
	defaultMetaTypes_.emplace_back( new UIntMetaType() );
	defaultMetaTypes_.emplace_back( new IntMetaType() );
	defaultMetaTypes_.emplace_back( new RealMetaType() );
	defaultMetaTypes_.emplace_back( new StringMetaType );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Collection >( "collection" ) );
	defaultMetaTypes_.emplace_back( new BinaryBlockSharedPtrMetaType() );
	defaultMetaTypes_.emplace_back( new Vector2MetaType() );
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
bool DefaultMetaTypeManager::deregisterType(const MetaType* type)
{
	assert( type != nullptr );
	const auto namesErased = typeNameToMetaType_.erase( type->name() );
	const auto typeInfosErased = typeInfoToMetaType_.erase( &type->typeInfo() );
	return (namesErased > 0) && (typeInfosErased > 0);
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

