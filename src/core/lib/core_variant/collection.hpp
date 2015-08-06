#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <iostream>
#include <iterator>
#include <type_traits>

#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include <cassert>

class Collection;

// collection and iterator implementations base

class CollectionIteratorImplBase;
typedef std::shared_ptr<CollectionIteratorImplBase> CollectionIteratorImplPtr;

class CollectionIteratorImplBase
{
public:
	typedef std::forward_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;

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

	virtual const TypeId & keyType() const = 0;
	virtual const TypeId & valueType() const = 0;
};

typedef std::shared_ptr<CollectionImplBase> CollectionImplPtr;


namespace details
{

	// linear collection

	template<typename Container>
	class LinearCollectionIteratorImpl:
		public CollectionIteratorImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::size_type key_type;
		typedef typename container_type::value_type value_type;
		typedef LinearCollectionIteratorImpl<container_type> this_type;

		static const bool is_variantType = 
			std::is_same<value_type, Variant>::value &&
			!std::is_const<container_type>::value &&
			!std::is_const<value_type>::value;

		static const bool can_set =
			variant::traits<value_type>::can_downcast &&
			!std::is_const<container_type>::value &&
			!std::is_const<value_type>::value;

		static const bool is_collection =
				!std::is_convertible<value_type, CollectionImplPtr>::value &&
				std::is_same<
				decltype(createCollectionImpl(std::declval<value_type&>())),
				CollectionImplPtr>::value;

		LinearCollectionIteratorImpl(container_type& container, key_type index):
			container_(container),
			index_(index)
		{
		}

		container_type& container() const
		{
			return container_;
		}

		key_type index() const
		{
			return index_;
		}


		Variant key() const override
		{
			return index_;
		}

		Variant value() const override
		{
			if(index_ < container_.size())
			{
				return container_[index_];
			}
			else
			{
				return Variant();
			}
		}

		bool setValue(const Variant& v) const override
		{
			return set_Value<is_variantType>(v);
		}

		void inc() override
		{
			index_ += 1;
		}

		bool equals(const CollectionIteratorImplBase& that) const override
		{
			const this_type* t = dynamic_cast<const this_type*>(&that);
			if(!t)
			{
				return false;
			}

			return
				&container_ == &t->container_ &&
				index_ == t->index_;
		}

		CollectionIteratorImplPtr clone() const override
		{
			return std::make_shared< this_type >(*this);
		}

	private:
		container_type& container_;
		key_type index_;

		template<bool is_variantType>
		bool set_Value( const Variant & v ) const
		{
			bool br = false;
			if(index_ < container_.size())
			{
				container_[index_] = v;
				br = true;
			}
			return br;
		}

		template<>
		bool set_Value<false>( const Variant & v ) const
		{
			return setValueImpl<can_set>(v);
		}

		template<bool can_set>
		bool setValueImpl(const Variant& v) const
		{
			bool br = false;
			if(index_ < container_.size())
			{
				br = v.with<value_type>([this](const value_type& val)
				{
					container_[index_] = val;
				});
			}
			return br;
		}

		template<>
		bool setValueImpl<false>(const Variant& v) const
		{
			// nop
			return false;
		}

	};


	template<typename Container>
	struct linear_collection_container_traits
	{
		typedef Container container_type;

		static void insertDefaultAt(container_type& container, const typename container_type::iterator& pos)
		{
			container.emplace(pos);
		}
	};

	template<typename Allocator>
	struct linear_collection_container_traits<std::vector<bool, Allocator>>
	{
		typedef std::vector<bool, Allocator> container_type;

		static void insertDefaultAt(container_type& container, const typename container_type::iterator& pos)
		{
			// std::vector<bool> doesn't have emplace
			container.insert(pos, false);
		}
	};


	template<typename Container, bool can_resize>
	class LinearCollectionImpl:
		public CollectionImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::size_type key_type;
		typedef typename container_type::value_type value_type;
		typedef LinearCollectionIteratorImpl<container_type> iterator_impl_type;

		template<bool can_set>
		struct downcaster_impl
		{
			static bool downcast(container_type* v, const Collection& storage)
			{
				if(v)
				{
					v->clear();
					v->reserve(storage.size());
					for(auto it = storage.begin(), end = storage.end(); it != end; ++it)
					{
						key_type i;
						if(!it.key().tryCast(i))
						{
							continue;
						}

						value_type val;
						if(!it.value().tryCast(val))
						{
							continue;
						}

						if(i >= v->size())
						{
							v->resize(i + 1);
						}

						(*v)[i] = val;
					}
				}

				return true;
			}
		};

		template<>
		struct downcaster_impl<false>
		{
		};

		typedef typename std::conditional<
			iterator_impl_type::can_set,
			downcaster_impl<iterator_impl_type::can_set>,
			void>::type downcaster;

		explicit LinearCollectionImpl(container_type& container):
			container_(container)
		{
		}


		const TypeId & keyType() const override
		{
			static auto s_KeyType = TypeId::getType< key_type >();
			return s_KeyType;
		}


		const TypeId & valueType() const override
		{
			static auto s_ValueType = TypeId::getType< value_type >();
			return s_ValueType;
		}


		bool empty() const override
		{
			return container_.empty();
		}


		size_t size() const override
		{
			return container_.size();
		}

		CollectionIteratorImplPtr begin() override
		{
			return std::make_shared< iterator_impl_type >(container_, 0);
		}

		CollectionIteratorImplPtr end() override
		{
			return std::make_shared< iterator_impl_type >(container_, container_.size());
		}

		std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy) override
		{
			typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

			key_type i;
			if(!key.tryCast(i))
			{
				return result_type(end(), false);
			}

			switch(policy)
			{
			case GET_EXISTING:
				if(i < container_.size())
				{
					return result_type(
						std::make_shared< iterator_impl_type >(container_, i), false);
				}
				else
				{
					return result_type(end(), false);
				}

			case GET_NEW:
				if(i > container_.size())
				{
					container_.resize(i + 1);
				}
				else
				{
					linear_collection_container_traits<container_type>::insertDefaultAt(container_, container_.begin() + i);
				}
			
				return result_type(
					std::make_shared< iterator_impl_type >(container_, i), true);

			case GET_AUTO:
				{
					bool found = i < container_.size();
					if(!found)
					{
						container_.resize(i + 1);
					}

					return result_type(
						std::make_shared< iterator_impl_type >(container_, i), !found);
				}

			default:
				return result_type(end(), false);

			}
		}

		CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos) override
		{
			iterator_impl_type* ii = dynamic_cast<iterator_impl_type*>(pos.get());
			assert(ii);
			assert(&ii->container() == &container_);
			assert(ii->index() < container_.size());

			auto r = container_.erase(container_.begin() + ii->index());
			return std::make_shared< iterator_impl_type >(
				container_,
				r - container_.begin());
		}

		size_t erase(const Variant& key) override
		{
			key_type i;
			if(!key.tryCast(i))
			{
				return 0;
			}

			if(i >= container_.size())
			{
				return 0;
			}

			container_.erase(container_.begin() + i);
			return 1;
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last) override
		{
			iterator_impl_type* ii_first = dynamic_cast<iterator_impl_type*>(first.get());
			iterator_impl_type* ii_last = dynamic_cast<iterator_impl_type*>(first.get());
			assert(ii_first && ii_last);
			assert(&ii_first->container() == &container_ && &ii_last->container() == &container_);
			assert(ii_first->index() < container_.size() && ii_last->index() < container_.size());

			auto r = container_.erase(
				container_.begin() + ii_first->index(), container_.begin() + ii_last->index());
			return std::make_shared< iterator_impl_type >(
				container_,
				r - container_.begin());
		}

	private:
		container_type& container_;

	};


	template<typename Container>
	class LinearCollectionImpl<Container, false>:
		public CollectionImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::size_type key_type;
		typedef typename container_type::value_type value_type;
		typedef LinearCollectionIteratorImpl<container_type> iterator_impl_type;

		template<bool can_set>
		struct downcaster_impl
		{
			static bool downcast(container_type* v, const Collection& storage)
			{
				if(v)
				{
					v->fill(value_type());
					for(auto it = storage.begin(), end = storage.end(); it != end; ++it)
					{
						key_type i;
						if(!it.key().tryCast(i))
						{
							continue;
						}

						if(i >= v->size())
						{
							continue;
						}

						value_type val;
						if(!it.value().tryCast(val))
						{
							continue;
						}

						(*v)[i] = val;
					}
				}

				return true;
			}
		};

		template<>
		struct downcaster_impl<false>
		{
		};

		typedef typename std::conditional<
			iterator_impl_type::can_set,
			downcaster_impl<iterator_impl_type::can_set>,
			void>::type downcaster;

		explicit LinearCollectionImpl(container_type& container):
			container_(container)
		{
		}


		const TypeId & keyType() const override
		{
			static auto s_KeyType = TypeId::getType< key_type >();
			return s_KeyType;
		}


		const TypeId & valueType() const override
		{
			static auto s_ValueType = TypeId::getType< value_type >();
			return s_ValueType;
		}


		bool empty() const override
		{
			return container_.empty();
		}


		size_t size() const override
		{
			return container_.size();
		}

		CollectionIteratorImplPtr begin() override
		{
			return std::make_shared< iterator_impl_type >(container_, 0);
		}

		CollectionIteratorImplPtr end() override
		{
			return std::make_shared< iterator_impl_type >(container_, container_.size());
		}

		std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy) override
		{
			typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

			key_type i;
			if(!key.tryCast(i))
			{
				return result_type(end(), false);
			}

			switch(policy)
			{
			case GET_EXISTING:
			case GET_AUTO:
				if(i < container_.size())
				{
					return result_type(
						std::make_shared< iterator_impl_type >(container_, i), false);
				}
				else
				{
					return result_type(end(), false);
				}

			case GET_NEW:
			default:
				return result_type(end(), false);

			}
		}

		CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos) override
		{
			return end();
		}

		size_t erase(const Variant& key) override
		{
			return 0;
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last) override
		{
			return end();
		}

	private:
		container_type& container_;

	};

	// map collection

	template<typename Map>
	class MapCollectionIteratorImpl:
		public CollectionIteratorImplBase
	{
	public:
		typedef typename Map::key_type key_type;
		typedef typename Map::mapped_type value_type;
		typedef Map container_type;
		typedef MapCollectionIteratorImpl<container_type> this_type;

		

		static const bool is_const_container = std::is_const<container_type>::value;
		static const bool can_set =
			variant::traits<value_type>::can_downcast &&
			!is_const_container &&
			!std::is_const<value_type>::value;

		static const bool is_variantType = 
			std::is_same<value_type, Variant>::value &&
			!is_const_container &&
			!std::is_const<value_type>::value;

		typedef typename std::conditional<
			is_const_container,
			typename container_type::const_iterator,
			typename container_type::iterator
		>::type iterator_type;

		MapCollectionIteratorImpl(container_type& container, const iterator_type& iterator):
			container_(container),
			iterator_(iterator)
		{
		}

		container_type& container() const
		{
			return container_;
		}

		const iterator_type& base() const
		{
			return iterator_;
		}

		Variant key() const override
		{
			if(iterator_ != container_.end())
			{
				return iterator_->first;
			}
			else
			{
				return Variant();
			}
		}

		Variant value() const override
		{
			if(iterator_ != container_.end())
			{
				return iterator_->second;
			}
			else
			{
				return Variant();
			}
		}

		bool setValue(const Variant& v) const override
		{
			return set_Value<is_variantType>(v);
		}

		void inc() override
		{
			++iterator_;
		}

		bool equals(const CollectionIteratorImplBase& that) const override
		{
			const this_type* t = dynamic_cast<const this_type*>(&that);
			if(!t)
			{
				return false;
			}

			return
				&container_ == &t->container_ &&
				iterator_ == t->iterator_;
		}

		CollectionIteratorImplPtr clone() const override
		{
			return std::make_shared< this_type >(*this);
		}

	private:
		container_type& container_;
		iterator_type iterator_;

		template<bool is_variantType>
		bool set_Value( const Variant & v ) const
		{
			bool br = false;
			if(iterator_ != container_.end())
			{
				iterator_->second = v;
				br = true;
			}
			return br;
		}

		template<>
		bool set_Value<false>( const Variant & v ) const
		{
			return setValueImpl<can_set>(v);
		}

		template<bool can_set>
		bool setValueImpl(const Variant& v) const
		{
			bool br = false;
			if(iterator_ != container_.end())
			{
				br = v.with<value_type>([this](const value_type& val)
				{
					iterator_->second = val;
				});
			}
			return br;
		}

		template<>
		bool setValueImpl<false>(const Variant& v) const
		{
			// nop
			return false;
		}

	};


	template<typename Map, bool can_resize>
	class MapCollectionImpl:
		public CollectionImplBase
	{
	public:
		typedef typename Map::key_type key_type;
		typedef typename Map::mapped_type value_type;
		typedef Map container_type;
		typedef MapCollectionIteratorImpl<container_type> iterator_impl_type;

		template<bool can_set>
		struct downcaster_impl
		{
			static bool downcast(container_type* v, const Collection& storage)
			{
				if(v)
				{
					v->clear();
					for(auto it = storage.begin(), end = storage.end(); it != end; ++it)
					{
						key_type i;
						if(!it.key().tryCast(i))
						{
							continue;
						}

						value_type val;
						if(!it.value().tryCast(val))
						{
							continue;
						}

						(*v)[i] = val;
					}
				}

				return true;
			}
		};

		template<>
		struct downcaster_impl<false>
		{
		};

		typedef typename std::conditional<
			iterator_impl_type::can_set,
			downcaster_impl<iterator_impl_type::can_set>,
			void>::type downcaster;

		explicit MapCollectionImpl(container_type& container):
			container_(container)
		{
		}


		const TypeId & keyType() const override
		{
			static auto s_KeyType = TypeId::getType< key_type >();
			return s_KeyType;
		}


		const TypeId & valueType() const override
		{
			static auto s_ValueType = TypeId::getType< value_type >();
			return s_ValueType;
		}


		bool empty() const override
		{
			return container_.empty();
		}


		size_t size() const override
		{
			return container_.size();
		}

		CollectionIteratorImplPtr begin() override
		{
			return std::make_shared< iterator_impl_type >(container_, container_.begin());
		}

		CollectionIteratorImplPtr end() override
		{
			return std::make_shared< iterator_impl_type >(container_, container_.end());
		}

		std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy) override
		{
			typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

			key_type k;
			if(!key.tryCast(k))
			{
				return result_type(end(), false);
			}

			switch(policy)
			{
			case GET_EXISTING:
				return result_type(
					std::make_shared< iterator_impl_type >(container_, container_.find(k)),
					false);

			case GET_NEW:
				{
					// insert a new one
					auto oldSize = container_.size();
					auto r = container_.emplace_hint(container_.upper_bound(k), k, value_type());
					if(container_.size() == oldSize)
					{
						return result_type(end(), false);
					}
					else
					{
						return result_type(
							std::make_shared< iterator_impl_type >(container_, r),
							true);
					}
				}

			case GET_AUTO:
				{
					auto range = container_.equal_range(k);
					if(range.first != range.second)
					{
						// key exists
						auto r = range.second;
						--r;
						return result_type(
							std::make_shared< iterator_impl_type >(container_, r), false);
					}

					// insert a new one
					auto r = container_.emplace_hint(range.second, k, value_type());
					return result_type(
						std::make_shared< iterator_impl_type >(container_, r),
						true);
				}

			default:
				return result_type(end(), false);

			}
		}

		CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos) override
		{
			iterator_impl_type* ii = dynamic_cast<iterator_impl_type*>(pos.get());
			assert(ii);
			assert(&ii->container() == &container_);

			return std::make_shared< iterator_impl_type >(
				container_,
				container_.erase(ii->base()));
		}

		size_t erase(const Variant& key) override
		{
			key_type k;
			if(!key.tryCast(k))
			{
				return 0;
			}

			return container_.erase(k);
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last) override
		{
			iterator_impl_type* ii_first = dynamic_cast<iterator_impl_type*>(first.get());
			iterator_impl_type* ii_last = dynamic_cast<iterator_impl_type*>(last.get());
			assert(ii_first && ii_last);
			assert(&ii_first->container() == &container_ && &ii_last->container() == &container_);

			return std::make_shared< iterator_impl_type >(
				container_,
				container_.erase(ii_first->base(), ii_last->base()));
		}

	private:
		container_type& container_;

	};


	template<typename Map>
	class MapCollectionImpl<Map, false>:
		public CollectionImplBase
	{
	public:
		typedef typename Map::key_type key_type;
		typedef typename Map::mapped_type value_type;
		typedef Map container_type;
		typedef MapCollectionIteratorImpl<container_type> iterator_impl_type;

		typedef void downcaster;

		explicit MapCollectionImpl(container_type& container):
			container_(container)
		{
		}


		const TypeId & keyType() const override
		{
			static auto s_keyType = TypeId::getType< key_type >();
			return s_keyType;
		}


		const TypeId & valueType() const override
		{
			static auto s_ValueType = TypeId::getType< value_type >();
			return s_ValueType;
		}


		bool empty() const override
		{
			return container_.empty();
		}


		size_t size() const override
		{
			return container_.size();
		}

		CollectionIteratorImplPtr begin() override
		{
			return std::make_shared< iterator_impl_type >(container_, container_.begin());
		}

		CollectionIteratorImplPtr end() override
		{
			return std::make_shared< iterator_impl_type >(container_, container_.end());
		}

		std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy) override
		{
			typedef std::pair<CollectionIteratorImplPtr, bool> result_type;

			key_type k;
			if(!key.tryCast(k))
			{
				return result_type(end(), false);
			}

			switch(policy)
			{
			case GET_EXISTING:
			case GET_AUTO:
				return result_type(
					std::make_shared< iterator_impl_type >(container_, container_.find(k)),
					false);

			case GET_NEW:
			default:
				return result_type(end(), false);

			}
		}

		CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos) override
		{
			return end();
		}

		size_t erase(const Variant& key) override
		{
			return 0;
		}

		CollectionIteratorImplPtr erase(
			const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last) override
		{
			return end();
		}

	private:
		container_type& container_;

	};

	// deduceCollectionImplType

	void deduceCollectionImplType(...);

	// std::vector 

	template<typename T, typename Alloc>
	LinearCollectionImpl<std::vector<T, Alloc>, true> deduceCollectionImplType(std::vector<T, Alloc>&);

	template<typename T, typename Alloc>
	LinearCollectionImpl<const std::vector<T, Alloc>, false> deduceCollectionImplType(const std::vector<T, Alloc>&);

	// std::array

	template<typename T, size_t N>
	LinearCollectionImpl<std::array<T, N>, false> deduceCollectionImplType(std::array<T, N>&);

	template<typename T, size_t N>
	LinearCollectionImpl<const std::array<T, N>, false> deduceCollectionImplType(const std::array<T, N>&);

	// std::map

	template<typename Key, typename T, typename Compare, typename Alloc>
	MapCollectionImpl<std::map<Key, T, Compare, Alloc>, true> deduceCollectionImplType(std::map<Key, T, Compare, Alloc>&);

	template<typename Key, typename T, typename Compare, typename Alloc>
	MapCollectionImpl<const std::map<Key, T, Compare, Alloc>, false> deduceCollectionImplType(const std::map<Key, T, Compare, Alloc>&);

	// std::unordered_map

	template<typename Key, typename T, typename Hash, typename Pred, typename Alloc>
	MapCollectionImpl<std::unordered_map<Key, T, Hash, Pred, Alloc>, true> deduceCollectionImplType(std::unordered_map<Key, T, Hash, Pred, Alloc>&);

	template<typename Key, typename T, typename Hash, typename Pred, typename Alloc>
	MapCollectionImpl<const std::unordered_map<Key, T, Hash, Pred, Alloc>, false> deduceCollectionImplType(const std::unordered_map<Key, T, Hash, Pred, Alloc>&);

	// std::multimap

	template<typename Key, typename T, typename Compare, typename Alloc>
	MapCollectionImpl<std::multimap<Key, T, Compare, Alloc>, true> deduceCollectionImplType(std::multimap<Key, T, Compare, Alloc>&);

	template<typename Key, typename T, typename Compare, typename Alloc>
	MapCollectionImpl<const std::multimap<Key, T, Compare, Alloc>, false> deduceCollectionImplType(const std::multimap<Key, T, Compare, Alloc>&);

	// std::unordered_multimap

	template<typename Key, typename T, typename Hash, typename Pred, typename Alloc>
	MapCollectionImpl<std::unordered_multimap<Key, T, Hash, Pred, Alloc>, true> deduceCollectionImplType(std::unordered_multimap<Key, T, Hash, Pred, Alloc>&);

	template<typename Key, typename T, typename Hash, typename Pred, typename Alloc>
	MapCollectionImpl<const std::unordered_multimap<Key, T, Hash, Pred, Alloc>, false> deduceCollectionImplType(const std::unordered_multimap<Key, T, Hash, Pred, Alloc>&);

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
		typename details::CollectionImpl<T>::type,
		void
	>::value,
	CollectionImplPtr
>::type createCollectionImpl(T& container)
{
	return std::make_shared< details::CollectionImpl<T>::type >(container);
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

	Note that this iterator implementation doesn't conform fully to standard
	iterator requirements.

	Note that operator*() could be slow because it returns a copy rather than
	a reference. Variant does not support pointers and references.
	*/
	class ConstIterator
	{
	public:
		typedef CollectionIteratorImplBase::iterator_category iterator_category;
		typedef Variant value_type;
		typedef CollectionIteratorImplBase::difference_type
			difference_type;

		// Variant does not support pointers and references
		typedef const Variant pointer;
		typedef const Variant reference;

		ConstIterator(const CollectionIteratorImplPtr& impl = CollectionIteratorImplPtr()):
			impl_(impl)
		{
		}

		Variant key() const
		{
			return impl_->key();
		}

		value_type value() const
		{
			return impl_->value();
		}

		bool setValue(const value_type& v)
		{
			// nop
			return false;
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

		// hide base implementation
		bool setValue(const Variant& v)
		{
			return impl()->setValue(v);
		}

		// hide base implementation
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

		typedef typename details::Downcaster<typename details::CollectionImpl<Container>::type>::type downcaster;

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
	TypeId keyType() const;
	/**
	Return TypeId of collection value
	*/
	TypeId valueType() const;

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

	Iterator erase(const Iterator& first, const Iterator& last);

	/**
	Access value associated by given key.
	*/
	ValueRef operator[](const Variant& key);

	/**
	Access value associated by given key.
	*/
	Variant operator[](const Variant& key) const;

	/**
	Test two collections equality.
	*/
	bool operator==(const Collection& that) const;

private:
	CollectionImplPtr impl_;

};


namespace variant
{

	// store any Collection compatible type in Variant as Collection
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

	template<typename T>
	typename std::enable_if<
		Collection::traits<T>::is_supported && Collection::traits<T>::can_downcast,
		bool
	>::type downcast(T* v, const Collection& storage)
	{
		return Collection::traits<T>::downcaster::downcast(v, storage);
	}

	template<typename T>
	typename std::enable_if<
		Collection::traits<T>::is_supported && !Collection::traits<T>::can_downcast,
		void
	>::type downcast(T* v, const Collection& storage);

	// don't try to store ValueRef in Variant, use ValueRef::operator Variant() instead
	void upcast(const Collection::ValueRef&);

}


template< typename T >
class CollectionHolder
	: public CollectionImplBase
{
public:
	CollectionHolder()
		: pImpl_( createCollectionImpl( collection_ ) )
	{
	}


	T & storage()
	{
		return collection_;
	}


	bool empty() const
	{
		return pImpl_->empty();
	}


	size_t size() const
	{
		return pImpl_->size();
	}


	CollectionIteratorImplPtr begin()
	{
		return pImpl_->begin();
	}


	CollectionIteratorImplPtr cbegin()
	{
		return pImpl_->cbegin();
	}


	CollectionIteratorImplPtr end()
	{
		return pImpl_->end();
	}


	CollectionIteratorImplPtr cend()
	{
		return pImpl_->cend();
	}


	std::pair<CollectionIteratorImplPtr, bool> get(const Variant& key, GetPolicy policy)
	{
		return pImpl_->get( key, policy );
	}

	CollectionIteratorImplPtr erase(const CollectionIteratorImplPtr& pos)
	{
		return pImpl_->erase( pos );
	}


	size_t erase(const Variant& key)
	{
		return pImpl_->erase( key );
	}

	CollectionIteratorImplPtr erase(
		const CollectionIteratorImplPtr& first, const CollectionIteratorImplPtr& last)
	{
		return pImpl_->erase( first, last );
	}

	const TypeId & keyType() const
	{
		return pImpl_->keyType();
	}


	const TypeId & valueType() const
	{
		return pImpl_->valueType();
	}

private:
	T collection_;
	CollectionImplPtr pImpl_;
};

#endif
