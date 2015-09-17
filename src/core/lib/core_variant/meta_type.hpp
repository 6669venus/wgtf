#ifndef META_TYPE_HPP
#define META_TYPE_HPP

#include "type_id.hpp"
#include <typeinfo>

class TextStream;
class BinaryStream;

/**
Base metatype class for any type that can be used inside Variant.
*/
class MetaType
{
public:
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
		DeducibleFromText = 2
	};

	MetaType(
		const char* name,
		size_t size,
		const std::type_info& typeInfo,
		const std::type_info* pointedType,
		int flags ):

		typeId_(typeInfo.name()),
		name_(name ? name : typeInfo.name()),
		size_(size),
		typeInfo_(typeInfo),
		pointedType_( pointedType ),
		flags_(flags)
	{
	}

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

	virtual void streamOut( TextStream& stream, const void* value ) const = 0;
	virtual void streamIn( TextStream& stream, void* value ) const = 0;
	
	virtual void streamOut( BinaryStream& stream, const void* value ) const = 0;
	virtual void streamIn( BinaryStream& stream, void* value ) const = 0;

	bool operator == ( const MetaType& other ) const
	{
		return typeId_ == other.typeId_ && strcmp(name_, other.name_) == 0;
	}
	

private:
	TypeId typeId_;
	const char* name_; // allow custom (human readable) type name for MetaType
	size_t size_;
	const std::type_info& typeInfo_;
	const std::type_info* pointedType_;
	int flags_;

};

#endif //META_TYPE_HPP
