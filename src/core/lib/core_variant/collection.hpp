#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include <iterator>
#include <type_traits>
#include <memory>

#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"


// collection and iterator implementations base

class CollectionIteratorImplBase;
typedef std::shared_ptr<CollectionIteratorImplBase> CollectionIteratorImplPtr;

class CollectionIteratorImplBase
{
public:
	typedef std::forward_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;

	virtual const TypeId& keyType() const = 0;
	virtual const TypeId& valueType() const = 0;
	virtual Variant key() const = 0;
	virtual Variant value() const = 0;
	virtual bool setValue(const Variant& v) const = 0;
	virtual void inc() = 0;
	virtual bool equals(const CollectionIteratorImplBase& that) const = 0;
	virtual CollectionIteratorImplPtr clone() const = 0;
};


class CollectionImplBase
{
public:
	// not just bool createNew for future support of multimaps
	enum GetPolicy
	{
		GET_EXISTING, // find existing element, returns end() if not found
		GET_NEW, // insert new element, returns end() if insertion failed
		GET_AUTO // find existing element or insert new one
	};

	virtual bool empty() const = 0;
	virtual size_t size() const = 0;
	virtual CollectionIteratorImplPtr begin() = 0;
	virtual CollectionIteratorImplPtr end() = 0;
	virtual std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy) = 0;
	virtual CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos) = 0;
	virtual size_t erase(const Variant& key) = 0;
	virtual CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last) = 0;

	virtual const TypeId& keyType() const = 0;
	virtual const TypeId& valueType() const = 0;

	virtual const TypeId& containerType() const = 0;
	virtual void* containerData() const = 0;
	/**
	 *	Check if the underlying container type is a map.
	 *	@return true if the container is a map.
	 */
	virtual bool isMapping() const = 0;
	/**
	 *	Check if the underlying container type can append/erase elements.
	 *	@return true if the container can change size.
	 */
	virtual bool canResize() const = 0;
};

typedef std::shared_ptr<CollectionImplBase> CollectionImplPtr;


namespace collection_details
{

	void deduceCollectionImplType(...);

}


#include "stl_container/linear_collection.hpp"
#include "stl_container/map_collection.hpp"


namespace collection_details
{

	template<typename Container>
	struct CollectionImpl
	{
		typedef decltype(deduceCollectionImplType(std::declval<Container&>())) type;
	};

	// downcaster

	template<typename Impl>
	struct Downcaster
	{
		typedef typename Impl::downcaster type;
	};

	template<>
	struct Downcaster<void>
	{
		typedef void type;
	};

}

/**
Helper function used to create collection implementation for given argument(s).
*/
void createCollectionImpl(...);

template<typename T>
typename std::enable_if<
	!std::is_same<
		typename collection_details::CollectionImpl<T>::type,
		void
	>::value,
	CollectionImplPtr
>::type createCollectionImpl(T& container)
{
	return std::make_shared< typename collection_details::CollectionImpl<T>::type >(container);
}


/**
Wrapper for generic container.
*/
class Collection
{
public:
	class Iterator;

	/**
	Proxy value that provides transparent read-write access to element value.
	*/
	class ValueRef
	{
	public:
		ValueRef(const CollectionIteratorImplPtr& impl):
			impl_(impl)
		{
		}

		template<typename T>
		bool tryCast(T& out) const
		{
			return impl_->value().tryCast(out);
		}

		template<typename T>
		T cast() const
		{
			return impl_->value().cast<T>();
		}

		template<typename T>
		const T& castRef() const
		{
			return impl_->value().castRef<T>();
		}

		template<typename T>
		T value() const
		{
			return impl_->value().value<T>();
		}

		template<typename T>
		T value(const T& def) const
		{
			return impl_->value().value<T>(def);
		}

		operator Variant() const
		{
			return impl_->value();
		}

		template<typename T>
		ValueRef& operator=(const T& v)
		{
			impl_->setValue(v);
			return *this;
		}

		template<typename T>
		bool operator==(const T& v) const
		{
			return impl_->value() == v;
		}

		template<typename T>
		bool operator!=(const T& v) const
		{
			return !(*this == v);
		}

	private:
		CollectionIteratorImplPtr impl_;

	};

	/**
	Read only forward iterator to collection element.

	@warning this iterator implementation doesn't conform fully to standard
	iterator requirements.

	@warning operator*() could be slow because it returns a copy rather than
	a reference.
	*/
	class ConstIterator
	{
	public:
		typedef CollectionIteratorImplBase::iterator_category iterator_category;
		typedef Variant value_type;
		typedef CollectionIteratorImplBase::difference_type
			difference_type;

		typedef const Variant pointer;
		typedef const Variant reference;

		ConstIterator(const CollectionIteratorImplPtr& impl = CollectionIteratorImplPtr()):
			impl_(impl)
		{
		}

		/**
		Type of element key.

		If Collection::keyType() reports Variant then this function will return
		the storage type of actual key. Otherwise results of both functions will
		match.
		*/
		const TypeId& keyType() const
		{
			return impl_->keyType();
		}

		/**
		Type of element value.

		If Collection::valueType() reports Variant then this function will
		return the storage type of actual value. Otherwise results of both
		functions will match.
		*/
		const TypeId& valueType() const
		{
			return impl_->valueType();
		}

		Variant key() const
		{
			return impl_->key();
		}

		value_type value() const
		{
			return impl_->value();
		}

		const CollectionIteratorImplPtr& impl() const
		{
			return impl_;
		}

		reference operator*() const
		{
			return value();
		}

		ConstIterator& operator++();

		ConstIterator operator++(int)
		{
			ConstIterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const ConstIterator& that) const;

		bool operator!=(const ConstIterator& that) const
		{
			return !(*this == that);
		}

	private:
		void detach();

		CollectionIteratorImplPtr impl_;

	};

	/**
	Read-write forward iterator to collection element.

	Note that this iterator implementation doesn't conform fully to standard
	iterator requirements.
	*/
	class Iterator:
		public ConstIterator
	{
	public:
		typedef ConstIterator base;

		typedef ConstIterator::iterator_category iterator_category;
		typedef ConstIterator::value_type value_type;
		typedef ConstIterator::difference_type difference_type;

		// Variant does not support pointers and references
		typedef ValueRef pointer;
		typedef ValueRef reference;

		Iterator(const CollectionIteratorImplPtr& impl = CollectionIteratorImplPtr()):
			base(impl)
		{
		}

		bool setValue(const Variant& v) const
		{
			return impl()->setValue(v);
		}

		reference operator*() const
		{
			return impl();
		}

		Iterator& operator++()
		{
			base::operator++();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

	};

	/**
	Check if specified container type is supported by Collection.
	*/
	template<typename Container>
	struct traits
	{
		static const bool is_supported =
			!std::is_convertible<Container, CollectionImplPtr>::value &&
			std::is_same<
				decltype(createCollectionImpl(std::declval<Container&>())),
				CollectionImplPtr>::value;

		typedef typename collection_details::Downcaster<typename collection_details::CollectionImpl<Container>::type>::type downcaster;

		static const bool can_downcast = !std::is_same<downcaster, void>::value;
	};

	/**
	Construct Collection using given implementation.
	*/
	explicit Collection(const CollectionImplPtr& impl = CollectionImplPtr()):
		impl_(impl)
	{
	}

	/**
	Construct Collection wrapper for given container.

	Note that current implementation allows only pointers to containers.
	*/
	template<typename Container>
	explicit Collection(Container& container, typename std::enable_if<traits<Container>::is_supported>::type* = nullptr):
		impl_(createCollectionImpl(container))
	{
	}

	/**
	Check whether Collection has reference to actual data.
	*/
	bool isValid() const;

	/**
	Return TypeId of collection key
	*/
	const TypeId& keyType() const;

	/**
	Return TypeId of collection value
	*/
	const TypeId& valueType() const;

	/**
	Try to cast underlying container pointer.
	*/
	template<typename Container>
	Container* container() const
	{
		if( !impl_ )
		{
			return nullptr;
		}

		const TypeId& containerType = impl_->containerType();
		if( containerType == TypeId::getType< Container >() ||
			( std::is_const< Container >::value &&
			containerType == TypeId::getType< typename std::remove_const< Container >::type >() ) )
		{
			return ( Container* )impl_->containerData();
		}

		return nullptr;
	}

	/**
	Check whether underlying container matches a given one.
	*/
	bool isSame( const void* container ) const;

	/**
	Check if collection is empty.
	*/
	bool empty() const;

	/**
	Returns elements count currently held in collection.
	*/
	size_t size() const;

	/**
	Iterator to the first element.
	*/
	Iterator begin();

	ConstIterator begin() const;

	ConstIterator cbegin() const
	{
		return begin();
	}

	/**
	Iterator to the imaginary element after the last one.
	*/
	Iterator end();

	ConstIterator end() const;

	ConstIterator cend() const
	{
		return end();
	}

	/**
	Find existing element.

	Returns iterator to the found element or end() if element with the given
	key doesn't exist.
	*/
	Iterator find(const Variant& key);

	ConstIterator find(const Variant& key) const;

	/**
	Try to insert a new element.

	Returns an iterator to the newly inserted element or end() if an element
	with the given key couldn't be inserted due to underlying container
	restrictions (e.g. element with given key already exists, or container
	doesn't support expanding).
	*/
	Iterator insert(const Variant& key);

	/**
	Find existing element or insert a new one.

	Returns a pair, with its member pair::first set to an iterator pointing to
	either the newly inserted element or to the element with an equivalent key
	in the collection. The pair::second element in the pair is set to @c true
	if a new element was inserted or @c false if an equivalent key already
	existed.

	Note that end() can still be returned if key is not found and insertion was
	failed.
	*/
	std::pair<Iterator, bool> get(const Variant& key);

	Iterator erase(const Iterator& pos);

	/**
	Erase all elements with given key.

	Returns amount of elements erased.
	*/
	size_t erase(const Variant& key);

	/**
	Erase elements between first and last, not including last.

	@return an iterator pointing to the position immediately following the last
		of the elements erased.
	*/
	Iterator erase(const Iterator& first, const Iterator& last);

	/**
	Access value associated by given key.
	*/
	ValueRef operator[](const Variant& key);

	/**
	Access value associated by given key.
	*/
	const Variant operator[](const Variant& key) const;

	/**
	Test two collections equality.
	*/
	bool operator==(const Collection& that) const;

	/**
	Test if the collection is a mapping.
	*/
	bool isMapping() const;

	/**
	Test if the collection can be resized larger or smaller.
	*/
	bool canResize() const;

	const CollectionImplPtr& impl() const
	{
		return impl_;
	}

private:
	CollectionImplPtr impl_;

};


namespace std
{

	// store compatible type from std namespace in Variant as Collection
	template<typename T>
	typename std::enable_if<Collection::traits<T>::is_supported, Collection>::type upcast(T&& v)
	{
		return Collection(v);
	}

	template<typename T>
	typename std::enable_if<Collection::traits<T>::is_supported, Collection>::type upcast(const T& v)
	{
		return Collection(v);
	}

}

template<typename T>
typename std::enable_if<
	Collection::traits<T>::is_supported && Collection::traits<T>::can_downcast,
	bool
>::type downcast(T* v, const Collection& storage)
{
	if(auto impl = storage.impl())
	{
		return Collection::traits<T>::downcaster::downcast(v, *impl);
	}
	else
	{
		return false;
	}
}

template<typename T>
typename std::enable_if<
	Collection::traits<T>::is_supported && !Collection::traits<T>::can_downcast,
	void
>::type downcast(T* v, const Collection& storage);

// don't try to store ValueRef in Variant, use ValueRef::operator Variant() instead
void upcast(const Collection::ValueRef&);


template< typename T >
class CollectionHolder:
	public collection_details::CollectionImpl<T>::type
{
	typedef typename collection_details::CollectionImpl<T>::type base;

public:
	CollectionHolder():
		base( collection_ ),
		collection_()
	{
	}

	T& storage()
	{
		return collection_;
	}

private:
	T collection_;

};

#endif

