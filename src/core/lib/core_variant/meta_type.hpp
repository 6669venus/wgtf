#ifndef META_TYPE_HPP
#define META_TYPE_HPP
#include "type_id.hpp"
#include <typeinfo>
/**
Base metatype class for any type that can be used inside Variant.
*/
class MetaType
{
public:
	enum Flag
	{
		ForceShared = 1 // Prefer dynamic storage and implicit sharing (shallow copy)
	};

	MetaType(const std::type_info& typeInfo, size_t size, const char* name = nullptr, int flags = 0):
		typeId_(typeInfo.name()),
		name_(name ? name : typeInfo.name()),
		size_(size),
		typeInfo_(typeInfo),
		pointedType_(nullptr),
		flags_(flags)
	{
	}

	MetaType(const std::type_info& typeInfo,
					 const std::type_info* pointedType,
					 const char* name = nullptr, int flags = 0):
		typeId_(typeInfo.name()),
		name_(name ? name : typeInfo.name()),
		size_(sizeof(void*)),
		typeInfo_(typeInfo),
		pointedType_(pointedType),
		flags_(flags)
	{
	}

	virtual ~MetaType() { }

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

	virtual bool streamOut(std::ostream& stream, const void* value) const = 0;
	virtual bool streamIn(std::istream& stream, void* value) const = 0;

private:
	TypeId typeId_;
	const char* name_; // allow custom (human readable) type name for MetaType
	size_t size_;
	const std::type_info& typeInfo_;
	const std::type_info* pointedType_;
	int flags_;

};

#endif //META_TYPE_HPP
