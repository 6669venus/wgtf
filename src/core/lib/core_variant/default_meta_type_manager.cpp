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

	static const std::string base64_chars = 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";


	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for(i = 0; (i <4) ; i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for(j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while((i++ < 3))
				ret += '=';

		}

		return ret;

	}

	std::string base64_decode(std::string const& encoded_string) {
		int in_len = static_cast<int>(encoded_string.size());
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i ==4) {
				for (i = 0; i <4; i++)
					char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j <4; j++)
				char_array_4[j] = 0;

			for (j = 0; j <4; j++)
				char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}

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
			std::string encodeValue = 
				base64_encode( static_cast<const unsigned char*>(binary->data()), 
							   static_cast<unsigned int>(binary->length()) );
			FixedMemoryStream dataStream( encodeValue.c_str(), encodeValue.length() );
			stream.serializeString( dataStream );
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			ResizingMemoryStream dataStream;
			stream.deserializeString( dataStream );
			std::string decodeValue = base64_decode( dataStream.buffer() );
			if (!stream.fail())
			{
				base::cast(value) = std::make_shared< BinaryBlock >(
					decodeValue.c_str(),
					decodeValue.length(),
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
}

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
	const auto typeInfosErased = typeInfoToMetaType_.erase( type->typeId() );
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

