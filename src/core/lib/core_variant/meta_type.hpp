#ifndef META_TYPE_HPP
#define META_TYPE_HPP

#include "type_id.hpp"
#include <typeinfo>
#include <unordered_map>

class TextStream;
class BinaryStream;

/**
Base metatype class for any type that can be used inside Variant.
*/
class MetaType
{
public:
	typedef bool (*ConversionFunc)( const MetaType* toType, void* to, const MetaType* fromType, const void* from );

	template< typename To, typename From >
	static bool straightConversion( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
	{
		To* t = reinterpret_cast< To* >( to );
		const From* f = reinterpret_cast< const From* >( from );

		*t = static_cast< To >( *f );

		return true;
	}

	enum Flag
	{
		/**
		Prefer dynamic storage and implicit sharing (shallow copy). Set this
		flag for types that are expensive to copy.
		*/
		ForceShared = 1,

		/**
		This type can be deduced from a textual representation. Currently only
		some basic types are deducible.
		*/
		DeducibleFromText = 2,

		/**
		Specifies whether this type is pointer to a const value.
		*/
		ConstPtr = 4
	};

	MetaType(
		const char* name,
		size_t size,
		const std::type_info& typeInfo,
		const std::type_info* pointedType,
		int flags );

	virtual ~MetaType()
	{
	}

	const TypeId& typeId() const
	{
		return typeId_;
	}

	const char* name() const
	{
		return name_;
	}

	size_t size() const
	{
		return size_;
	}

	const std::type_info& typeInfo() const
	{
		return typeInfo_;
	}

	virtual void init(void* value) const = 0;
	virtual void copy(void* dest, const void* src) const = 0;
	virtual void move(void* dest, void* src) const = 0;
	virtual void destroy(void* value) const = 0;
	virtual bool equal(const void* lhs, const void* rhs) const = 0;

	const std::type_info* pointedType() const
	{
		return pointedType_;
	}

	int flags() const
	{
		return flags_;
	}

	bool testFlags( int test ) const
	{
		return ( flags_ & test ) == test;
	}

	virtual void streamOut( TextStream& stream, const void* value ) const = 0;
	virtual void streamIn( TextStream& stream, void* value ) const = 0;
	
	virtual void streamOut( BinaryStream& stream, const void* value ) const = 0;
	virtual void streamIn( BinaryStream& stream, void* value ) const = 0;

	/**
	Try convert a @a from value of @a fromType to a @a to value of this type.
	*/
	bool convertFrom( void* to, const MetaType* fromType, const void* from ) const;

	/**
	Try convert a @a from value of this type to a @a to value of @a toType.
	*/
	bool convertTo( const MetaType* toType, void* to, const void* from ) const;

	bool operator == ( const MetaType& other ) const
	{
		return typeId_ == other.typeId_ && strcmp(name_, other.name_) == 0;
	}

protected:
	void addConversionFrom( const std::type_info& fromType, ConversionFunc func );

	template< typename From >
	void addConversionFrom( ConversionFunc func )
	{
		addConversionFrom( typeid( From ), func );
	}

	template< typename To, typename From >
	void addStraightConversion()
	{
		addConversionFrom( typeid( From ), &straightConversion< To, From > );
	}

	void setDefaultConversionFrom( ConversionFunc func );

private:
	struct TypeInfoHash
	{
		size_t operator()(const std::type_info* v) const
		{
			return v->hash_code();
		}
	};

	struct TypeInfosEq
	{
		bool operator()(const std::type_info* lhs, const std::type_info* rhs) const
		{
			return
				lhs == rhs ||
				*lhs == *rhs;
		}
	};

	TypeId typeId_;
	const char* name_; // allow custom (human readable) type name for MetaType
	size_t size_;
	const std::type_info& typeInfo_;
	const std::type_info* pointedType_;
	int flags_;
	std::unordered_map< const std::type_info*, ConversionFunc, TypeInfoHash, TypeInfosEq > conversionsFrom_;
	ConversionFunc defaultConversionFrom_;

};

#endif //META_TYPE_HPP
