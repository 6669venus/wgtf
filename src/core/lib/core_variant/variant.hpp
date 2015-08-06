#ifndef VARIANT_HPP_INCLUDED
#define VARIANT_HPP_INCLUDED

#include <string>
#include <typeinfo>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <memory>

#include <cstdint>

#include "type_id.hpp"
#include "meta_type.hpp"
#include "interfaces/i_meta_type_manager.hpp"

class Variant;

/**
This namespace contains Variant type specific stuff.
*/
namespace variant
{

	namespace details
	{

		/**
		Placeholder type used in templates below to select default behaviour.
		*/
		struct Default
		{
		};

		/**
		Implementation details of type traits used by Variant.
		*/
		template<typename T>
		struct TraitsImpl
		{
			typedef typename std::decay<T>::type value_type;

			/*
			Deduce upcast result type. If upcast returns Default then assume
			pass-through upcast (which just returns its argument).
			*/
			typedef decltype(::variant::upcast(std::declval<T>())) raw_direct_upcasted_type;
			typedef typename std::conditional<
				std::is_same<raw_direct_upcasted_type, Default>::value,
				T, // we can safely pass input type as-is, since it's used as return type only of default (non-recursive) upcast
				raw_direct_upcasted_type
			>::type direct_upcasted_type;

			typedef typename std::decay<direct_upcasted_type>::type direct_storage_type; // storage type is deduced from upcast return value

			/**
			Helper struct used for resursive up- and downcast.
			Default version implements recursive cast and type deduction.
			*/
			template<bool recur>
			struct recursion_helper_impl
			{
				typedef TraitsImpl<direct_upcasted_type> recursive_traits;

				typedef typename recursive_traits::upcasted_type upcasted_type;
				typedef typename std::decay<upcasted_type>::type storage_type;
				typedef typename std::decay<decltype(::variant::downcast(std::declval<value_type*>(), std::declval<direct_storage_type>()))>::type direct_downcast_result_type;

				static const bool can_upcast = recursive_traits::can_upcast;
				static const bool can_downcast = recursive_traits::can_downcast;

				template<typename U>
				static upcasted_type upcast(U&& v)
				{
					return recursive_traits::recursion_helper::upcast(::variant::upcast(std::forward<U>(v)));
				}

				static bool downcast(value_type* v, const storage_type* storage)
				{
					direct_storage_type tmp;
					if(!recursive_traits::recursion_helper::downcast(&tmp, storage))
					{
						return false;
					}

					return ::variant::downcast(v, tmp);
				}
			};

			/**
			Recursion-stopper specialization.
			*/
			template<>
			struct recursion_helper_impl<false>
			{
				typedef direct_upcasted_type upcasted_type;
				typedef typename std::decay<upcasted_type>::type storage_type;
				typedef bool direct_downcast_result_type;

				static const bool can_upcast = true;
				static const bool can_downcast = true;

				template<typename U>
				static upcasted_type upcast(U&& v)
				{
					return std::forward<U>(v);
				}

				static bool downcast(value_type* v, const storage_type* storage)
				{
					if(v)
					{
						*v = *storage;
					}

					return true;
				}
			};

			static const bool upcasted_to_void = std::is_same<direct_storage_type, void>::value;
			static const bool recur =
				!upcasted_to_void &&
				!std::is_same<direct_storage_type, value_type>::value;

			typedef recursion_helper_impl<recur> recursion_helper;
			typedef typename recursion_helper::upcasted_type upcasted_type;
			typedef typename recursion_helper::storage_type storage_type;
			typedef typename recursion_helper::direct_downcast_result_type direct_downcast_result_type;

			static const bool can_upcast =
				!upcasted_to_void && // don't allow upcast if storage is void
				!std::is_convertible<value_type, const MetaType*>::value && // reject MetaType* (there's special constructor for it),
				!std::is_same<value_type, Variant>::value && // reject Variant type itself
				recursion_helper::can_upcast; // check whole upcast chain

			static const bool can_downcast =
				can_upcast && // if value can't be upcasted then corresponding downcast doesn't make any sense
				std::is_same<direct_downcast_result_type, bool>::value &&
				recursion_helper::can_downcast; // check whole downcast chain

		private:
			struct Yes {};
			struct No {};
	
			template<typename U>
			static Yes checkStreamingOut(typename std::remove_reference<decltype(std::declval<std::ostream&>() << std::declval<const U&>())>::type*);
	
			template<typename U>
			static No checkStreamingOut(...);
	
			template<typename U>
			static Yes checkStreamingIn(typename std::remove_reference<decltype(std::declval<std::istream&>() >> std::declval<U&>())>::type*);
	
			template<typename U>
			static No checkStreamingIn(...);
	
		public:
			static const bool has_streaming_out = std::is_same<decltype(checkStreamingOut<T>(0)), Yes>::value;
			static const bool has_streaming_in = std::is_same<decltype(checkStreamingIn<T>(0)), Yes>::value;

		};

		/**
		Helper struct that provides recursive downcast implementation in case
		the whole downcast chain is valid.
		*/
		template<typename T, bool can_downcast>
		class Downcaster
		{
			typedef TraitsImpl<T> traits_impl;

		public:
			static bool downcast(typename traits_impl::value_type* v, const typename traits_impl::storage_type& storage)
			{
				return traits_impl::recursion_helper::downcast(v, &storage);
			}
		};

		template<typename T>
		class Downcaster<T, false>
		{
		};

		/**
		Helper struct that provides recursive upcast implementation in case
		the whole upcast chain is valid.
		*/
		template<typename T, bool can_upcast>
		class Upcaster
		{
			typedef TraitsImpl<T> traits_impl;

		public:
			template<typename U>
			static typename traits_impl::upcasted_type upcast(U&& v)
			{
				return traits_impl::recursion_helper::upcast(std::forward<U>(v));
			}

		};

		template<typename T>
		class Upcaster<T, false>
		{
		};

		/**
		Helper struct that provides access to either existing streaming-out
		implementation or error-stub.
		*/
		template<typename T, bool has_streaming_out>
		struct StreamerOut
		{
			static bool streamOut(std::ostream& stream, const T& value)
			{
				stream << value;
				return stream.good();
			}
		};

		template<typename T>
		struct StreamerOut<T, false>
		{
			static bool streamOut(std::ostream& stream, const T&)
			{
				stream.setstate(std::ios_base::failbit);
				return false;
			}
		};

		/**
		Helper struct that provides access to either existing streaming-in
		implementation or error-stub.
		*/
		template<typename T, bool has_streaming_in>
		struct StreamerIn
		{
			static bool streamIn(std::istream& stream, T& value)
			{
				stream >> value;
				return !stream.fail();
			}
		};

		template<typename T>
		struct StreamerIn<T, false>
		{
			static bool streamIn(std::istream& stream, T&)
			{
				stream.setstate(std::ios_base::failbit);
				return false;
			}
		};

	}

	/**
	Helper function used to reduce argument type to storage type.

	This function (and its overloads) is used to get storage type for a bunch
	of other compatible types. This allows to register one type and use it to
	store a range of compatible types.

	Argument type itself is used as storage by default.

	You can define your own overloads for your custom types.
	*/
	details::Default upcast(...);

	/**
	Helper function used to cast stored value to some other type.

	Implicit cast is used by default.

	You can define your own overloads for your custom types. If overload returns
	other type than @c bool then this overload will never be called and
	corresponding wil be rejected (at compile time).
	*/
	template<typename T, typename Storage>
	bool downcast(T* v, const Storage& storage)
	{
		if(v)
		{
			*v = storage;
		}
		return true;
	}

	/**
	Helper function used to compare stored value with some value.

	Default implementation performs comparison using operator==().

	You can define your own overloads for your custom types.
	*/
	template<typename Storage, typename Value>
	bool equal(const Storage& s, const Value& v)
	{
		return s == v;
	}

	/**
	Helper structure used to get useful information about types Variant works with.
	*/
	template<typename T>
	struct traits:
		public details::Upcaster<T, details::TraitsImpl<T>::can_upcast>,
		public details::Downcaster<T, details::TraitsImpl<T>::can_downcast>,
		public details::StreamerOut<T, details::TraitsImpl<T>::has_streaming_out>,
		public details::StreamerIn<T, details::TraitsImpl<T>::has_streaming_in>
	{
		typedef details::TraitsImpl<T> traits_impl;

	public:
		typedef typename traits_impl::value_type value_type;
		typedef typename traits_impl::upcasted_type upcasted_type;
		typedef typename traits_impl::storage_type storage_type;

		static const bool can_upcast = traits_impl::can_upcast;
		static const bool can_downcast = traits_impl::can_downcast;
	};

	// uint64_t

	inline uint64_t upcast(uint64_t v) { return v; }
	inline uint64_t upcast(uint32_t v) { return v; }
	inline uint64_t upcast(uint16_t v) { return v; }
	inline uint64_t upcast(uint8_t v) { return v; }

	template<typename T>
	bool downcast(T* v, uint64_t storage)
	{
		if(v)
		{
			*v = static_cast<T>(storage);
		}
		return true;
	}

	// int64_t

	inline int64_t upcast(int64_t v) { return v; }
	inline int64_t upcast(int32_t v) { return v; }
	inline int64_t upcast(int16_t v) { return v; }
	inline int64_t upcast(int8_t v) { return v; }

	template<typename T>
	bool downcast(T* v, int64_t storage)
	{
		if(v)
		{
			*v = static_cast<T>(storage);
		}
		return true;
	}

	// bool

	/*
	Don't uncomment following line as many types (pointers, custom types with
	cast-to-bool operator) will be implicitly converted to bool. However bool
	itself is promoted to int (see C++ standard, conv.prom/6), so its storage
	is int64_t.
	*/
	//inline int64_t upcast(bool v) { return v; }

	inline bool downcast(bool* v, int64_t storage)
	{
		if(v)
		{
			*v = (storage != 0);
		}
		return true;
	}

	inline bool equal(int64_t s, bool v)
	{
		return (s != 0) == v;
	}

	// double

	inline double upcast(double v) { return v; }
	inline double upcast(float v) { return v; }

	inline bool downcast(float* v, double storage)
	{
		if(v)
		{
			*v = static_cast<float>(storage);
		}
		return true;
	}

	// std::string
	std::string upcast(const char* v);
	inline std::string upcast(std::string&& v) { return v; }
	inline const std::string& upcast(const std::string & v) { return v; }
	std::string upcast(const std::wstring& v);
	std::string upcast(const wchar_t* v);

	bool downcast(std::wstring* v, const std::string& storage);

	// forbid casting to raw string pointers as string lifetime is subject of Variant internals
	void downcast(const char** v, const std::string& storage);
	void downcast(const wchar_t** v, const std::string& storage);

	inline bool equal(const std::string& s, const std::wstring& v)
	{
		return (s == upcast( v ));
	}

	inline bool equal(const std::string& s, const wchar_t * v)
	{
		return (s == upcast( v ));
	}

	// shared_ptr

	/*template<typename T>
	T* upcast(const std::shared_ptr<T>& v)
	{
		return v.get();
	}

	// Deny downcasting to shared_ptr.
	// When you reset shared_ptr by raw pointer a NEW reference counter is
	// created. And if the same pointer is managed by two (or even more) shared
	// counters you will eventually get multiple deallocations of the same
	// object. So just don't do this.
	//TODO: Look into std::enabled_shared_for_this
	template<typename T>
	void downcast(std::shared_ptr<T>* v, T* storage);*/

}

class Variant
{
	friend std::ostream& operator<<(std::ostream& stream, const Variant& value);
	friend std::istream& operator>>(std::istream& stream, Variant& value);

public:
	/**
	Construct @c void (i.e. empty) variant.
	*/
	Variant();

	/**
	Construct a copy.
	Variant copy has the same type and value as another one.
	*/
	Variant(const Variant& value);

	/**
	Move variant value.
	*/
	Variant(Variant&& value);

	/**
	Construct variant of specified type with default value.
	*/
	explicit Variant(const MetaType* type);

	/**
	Construct variant from a given value.
	If value type is not registered then @c std::bad_cast is thrown.

	@see registerType
	*/
	template<typename T>
	Variant(T&& value, typename std::enable_if<variant::traits<T>::can_upcast>::type* = nullptr)
	{
		init(std::forward<T>(value));
	}

	/**
	Destroy underlying value and free all resources used by this Variant.
	*/
	~Variant()
	{
		destroy();
	}

	/**
	Assign another variant value to this one.
	*/
	Variant& operator=(const Variant& value);

	/**
	Move another variant value to this one.
	*/
	Variant& operator=(Variant&& value);

	/**
	Assign given value to the Variant.
	If value type is not registered then @c std::bad_cast exception is thrown.

	@see registerType
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_upcast, Variant&>::type operator=(T&& value)
	{
		if(typeIs<T>())
		{
			detach();
			assign(std::forward<T>(value));
		}
		else
		{
			destroy();
			init(std::forward<T>(value));
		}

		return *this;
	}

	/**
	Compare two variants.

	Built-in types (and all supported conversions) may be cross-compared,
	i.e. @c int may be compared with @a double.

	User types (i.e. registered outside) may be compared only with each
	other, their native support (i.e. comparison operator overloads) of
	comparing with other types doesn't matter. Different user types will
	never match.

	Also any type may be compared with string. Comparison succeeds if string
	deserialization to a corresponding type succeeds and resulting values match.
	*/
	bool operator==(const Variant& v) const;

	bool operator!=(const Variant& v) const
	{
		return !(*this == v);
	}

	/**
	Compare variant with arbitrary value.
	
	Built-in and user types are supported.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_upcast, bool>::type operator==(const T& value) const
	{
		typedef typename variant::traits<T>::storage_type storage_type;

		bool equal = false;
		with<storage_type>([&](const storage_type& v)
		{
			equal = variant::equal(v, value);
		});

		return equal;
	}

	template<typename T>
	typename std::enable_if<variant::traits<T>::can_upcast, bool>::type operator!=(const T& value) const
	{
		return !(*this == value);
	}

	/**
	Type of currently stored value.
	*/
	const MetaType* type() const
	{
		return type_;
	}

	/**
	Check if variant has @c void type (i.e. it has no value).
	*/
	bool isVoid() const;

	/**
	Check if variant contains value of pointer type.
	Any pointer type can be cast to <tt>void*</tt> type.
	*/
	bool isPointer() const;

	/**
	Check if current value has exact type.
	*/
	template<typename T>
	bool typeIs() const
	{
		return type_ == findType<T>();
	}

	/**
	Check if current value may be casted to the given type.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_downcast, bool>::type canCast() const
	{
		return tryCastImpl((T*)nullptr);
	}

	/**
	Try to cast current value to the given type.
	Returns @c true if cast succeeded, @c false otherwise.
	If cast fails then output value is left intact.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_downcast, bool>::type tryCast(T& out) const
	{
		return tryCastImpl(&out);
	}

	/**
	Try to cast current value to the given type without any conversions.
	If cast fails then @c std::bad_cast exception is thrown.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_downcast, const T&>::type castRef() const
	{
		if(!typeIs<T>())
		{
			castError();
		}

		return forceCast<T>();
	}

	/**
	Try to cast current value to the given type.
	If cast fails then @c std::bad_cast exception is thrown.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_downcast, T>::type cast() const
	{
		T result;
		if(!tryCastImpl(&result))
		{
			castError();
		}

		return result;
	}

	/**
	Try to cast current value to the given type.
	If cast fails then value constructed by default is returned.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_downcast, T>::type value() const
	{
		T result = T();

		tryCastImpl(&result);

		return result;
	}

	/**
	Try to cast current value to the given type.
	If cast fails then the given default value is returned.
	*/
	template<typename T>
	typename std::enable_if<variant::traits<T>::can_downcast, T>::type value(const T& def) const
	{
		T result;

		if(!tryCastImpl(&result))
		{
			result = def;
		}

		return result;
	}

	/**
	Call given function with current value casted to the given type.

	This function allows to avoid value copy when possible.

	@return @c true if cast was succeeded and the function was called, @c false if
	cast was failed and the function was not called.
	*/
	template<typename T, typename Fn>
	typename std::enable_if<variant::traits<T>::can_downcast, bool>::type with(const Fn& fn) const
	{
		if(typeIs<T>())
		{
			fn(forceCast<T>());
			return true;
		}

		T tmp;
		if(tryCastImpl(&tmp))
		{
			fn(tmp);
			return true;
		}

		return false;
	}

	//Must be used before any other function on Variant.
	static void setMetaTypeManager( IMetaTypeManager * metaTypeManager );
	static IMetaTypeManager * getMetaTypeManager();

	/**
	Register type in current metatype manager (if any).
	*/
	static bool registerType(const MetaType* type);

	/**
	Templated version of findType.

	@see IMetaTypeManager::findType
	*/
	template<typename T>
	static const MetaType* findType()
	{
		return findTypeImpl<typename std::decay<T>::type>();
	}

	/**
	Check whether given type is registered.

	@see findType
	*/
	template<typename T>
	static bool typeIsRegistered()
	{
		return findType<T>() != nullptr;
	}

	/**
	Utility function for string serialization.
	*/
	static bool streamOut(std::ostream& stream, const std::string& value);

	/**
	Utility function for pointer serialization.
	*/
	static bool streamOut(std::ostream& stream, void* value);

	/**
	Utility function for string deserialization.
	*/
	static bool streamIn(std::istream& stream, std::string& value);

	/**
	Utility function for pointer deserialization.
	*/
	static bool streamIn(std::istream& stream, void*& value);

private:
	static const size_t INLINE_PAYLOAD_SIZE = 16; // sizeof(Collection)

	class DynamicData
	{
	public:
		template<typename T>
		static DynamicData* allocate()
		{
			return allocate(sizeof(T));
		}

		static DynamicData* allocate(size_t payloadSize);

		void incRef()
		{
			refs_ += 1;
		}

		void decRef(const MetaType* type);

		/**
		Check if there's only one reference to this data.
		*/
		bool isExclusive() const
		{
			return refs_ == 0;
		}

		void* payload()
		{
			return this + 1;
		}

	private:
		DynamicData():
			refs_(0)
		{
		}

		mutable volatile unsigned refs_; // contains actual references minus one

	};

	/**
	The way data is stored depends on payload (i.e. stored type) size.
	If payload size fits into INLINE_PAYLOAD_SIZE then it's stored in inline
	payload. Otherwise dynamically allocated storage is used.

	Dynamically allocated storage is shared among all variant copies.
	*/
	union Data
	{
		char payload_[INLINE_PAYLOAD_SIZE];
		DynamicData* dynamic_;
	};

	const MetaType* type_;
	Data data_;

	bool isInline() const
	{
		return
			type_->size() <= INLINE_PAYLOAD_SIZE &&
			(type_->flags() & MetaType::ForceShared) == 0;
	}

	const void* payload() const
	{
		if(isInline())
		{
			return data_.payload_;
		}
		else
		{
			return data_.dynamic_->payload();
		}
	}

	void* payload()
	{
		if(isInline())
		{
			return data_.payload_;
		}
		else
		{
			return data_.dynamic_->payload();
		}
	}

	template<typename T>
	const T& forceCast() const
	{
		return *reinterpret_cast<const T*>(payload());
	}

	template<typename T>
	T& forceCast()
	{
		return *reinterpret_cast<T*>(payload());
	}

	template<typename T>
	static const MetaType* findTypeImpl()
	{
		return getMetaTypeManager()->findType(typeid(T));
	}

	template<typename T>
	void init( T&& value )
	{
		typedef typename variant::traits<T>::storage_type storage_type;

		type_ = findType<storage_type>();
		const IStorageLookupHandler * handler = nullptr;
		static TypeId typeId = TypeId::getType< T >();
		if(type_ == nullptr)
		{
			handler = getMetaTypeManager()->dynamicStorageHandlerLookup(
				typeId );
			type_ = handler ? handler->findStorageType( typeId ) : nullptr;
			if(type_ == nullptr)
			{
				typeInitError();
			}
		}

		void* p;
		if(isInline())
		{
			p = data_.payload_;
		}
		else
		{
			if (handler)
			{
				data_.dynamic_ = DynamicData::allocate( handler->storageSize());
			}
			else
			{
				data_.dynamic_ = DynamicData::allocate<storage_type>();
			}
			p = data_.dynamic_->payload();
		}

		if (handler)
		{
			handler->initStorage( p, typeId, &value );
		}
		else
		{
			new (p) storage_type(variant::traits<T>::upcast(std::forward<T>(value)));
		}
	}

	void init(const Variant& value);
	void init(Variant&& value);

	template<typename T>
	typename std::enable_if<std::is_rvalue_reference<T>::value>::type assign(T&& value)
	{
		type_->move(payload(), &value);
	}

	template<typename T>
	typename std::enable_if<!std::is_rvalue_reference<T>::value>::type assign(const T& value)
	{
		type_->copy(payload(), &value);
	}

	bool tryCastFromString(const MetaType* destType, void* dest) const;

	template<typename T>
	bool tryCastFromString(T* out = nullptr) const
	{
		if(!typeIs<std::string>())
		{
			return false;
		}

		const MetaType* destType = findType<T>();
		if(!destType)
		{
			return false;
		}

		if(out)
		{
			return tryCastFromString(destType, out);
		}
		else
		{
			// allocate temporary as implementaion requires target value
			T tmp;
			return tryCastFromString(destType, &tmp);
		}
	}

	template<typename T>
	bool tryCastImpl(T* out) const
	{
		typedef typename variant::traits<T>::value_type value_type;
		typedef typename variant::traits<T>::storage_type storage_type;

		if(typeIs<storage_type>())
		{
			return variant::traits<T>::downcast(out, forceCast<storage_type>());
		}

		TypeId typeId = TypeId::getType< T >();
		auto handler =
			getMetaTypeManager()->dynamicStorageHandlerLookup( typeId );
		if (handler)
		{
			try
			{
				handler->tryConvert(
					TypeId( type_->name() ), payload() );
			}
			catch( const T * converted )
			{
				*out = *converted;
				return true;
			}
			catch (...)
			{
				// Not expected type
			}
		}
		storage_type tmp;
		if(std::is_same<value_type, storage_type>::value)
		{
			// it's not one of built-in types (see below)
			if(!tryCastFromString(&tmp))
			{
				return false;
			}
		}
		else
		{
			// try again with storage
			if(!tryCastImpl(&tmp))
			{
				return false;
			}
		}
		return variant::traits<T>::downcast(out, tmp);
	}

	bool tryCastImpl(uint64_t* out) const;
	bool tryCastImpl(int64_t* out) const;
	bool tryCastImpl(double* out) const;
	bool tryCastImpl(std::string* out) const;
	bool tryCastImpl(void** out) const;

	void destroy();
	void detach();

	static void castError();
	static void typeInitError();

};


/**
Allow Variant to be streamed out.
*/
std::ostream& operator<<(std::ostream& stream, const Variant& value);

/**
Allow Variant to be streamed in.
*/
std::istream& operator>>(std::istream& stream, Variant& value);




/**
Default implementation of MetaType.
*/
template<typename T>
class MetaTypeImpl:
	public MetaType
{
	typedef MetaType base;
	typedef T value_type;

public:
	explicit MetaTypeImpl(const char* name = nullptr, int flags = 0):
		base(typeid(value_type), sizeof(value_type), name, flags)
	{
	}

	void init(void* value) const override
	{
		new (value) value_type();
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
		cast(value)->~value_type();
	}

	bool equal(const void* lhs, const void* rhs) const override
	{
		return *cast(lhs) == *cast(rhs);
	}

	bool streamOut(std::ostream& stream, const void* value) const override
	{
		return variant::traits<value_type>::streamOut(stream, *cast(value));
	}

	bool streamIn(std::istream& stream, void* value) const override
	{
		return variant::traits<value_type>::streamIn(stream, *cast(value));
	}

private:
	static value_type* cast(void* value)
	{
		return static_cast<value_type*>(value);
	}

	static const value_type* cast(const void* value)
	{
		return static_cast<const value_type*>(value);
	}

};


/**
Specialization for pointer types.
*/
template<typename T>
class MetaTypeImpl<T*>:
	public MetaType
{
	typedef MetaType base;
	typedef T* value_type;

public:
	explicit MetaTypeImpl(const char* name = nullptr, int flags = 0):
		base(typeid(value_type), &typeid(T), name, flags)
	{
	}

	void init(void* value) const override
	{
		new (value) value_type();
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
		cast(value)->~value_type();
	}

	bool equal(const void* lhs, const void* rhs) const override
	{
		return *cast(lhs) == *cast(rhs);
	}

	bool streamOut(std::ostream& stream, const void* value) const override
	{
		return Variant::streamOut(stream, *static_cast<void* const*>(value));
	}

	bool streamIn(std::istream& stream, void* value) const override
	{
		return Variant::streamIn(stream, *static_cast<void**>(value));
	}

private:
	static value_type* cast(void* value)
	{
		return static_cast<value_type*>(value);
	}

	static const value_type* cast(const void* value)
	{
		return static_cast<const value_type*>(value);
	}

};


#endif