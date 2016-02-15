#ifndef MAP_CONTAINER_COLLECTION_HPP
#define MAP_CONTAINER_COLLECTION_HPP

#include <map>
#include <unordered_map>
#include <cassert>
#include <utility>


namespace collection_details
{

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
			Variant::traits<value_type>::can_downcast &&
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

		const TypeId& keyType() const override
		{
			return GetTypeImpl<key_type>::keyType(this);
		}

		const TypeId& valueType() const override
		{
			return GetTypeImpl<value_type>::valueType(this);
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
			return SetImpl<can_set>::setValue(this, v);
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

		// SetImpl
		template<bool can_set, typename Dummy = void>
		struct SetImpl
		{
			static bool setValue(const this_type* impl, const Variant& v)
			{
				if(impl->iterator_ == impl->container_.end())
				{
					return false;
				}

				return v.with<value_type>([impl](const value_type& val)
				{
					impl->iterator_->second = val;
				});
			}
		};

		template<typename Dummy>
		struct SetImpl<false, Dummy>
		{
			static bool setValue(const this_type* impl, const Variant& v)
			{
				// nop
				return false;
			}
		};

		// GetTypeImpl
		template<typename T, typename Dummy = void>
		struct GetTypeImpl
		{
			static const TypeId& keyType(const this_type*)
			{
				return TypeId::getType<T>();
			}

			static const TypeId& valueType(const this_type*)
			{
				return TypeId::getType<T>();
			}
		};

		template<typename Dummy>
		struct GetTypeImpl<Variant, Dummy>
		{
			static const TypeId& keyType(const this_type* impl)
			{
				if(impl->iterator_ != impl->container_.end())
				{
					return impl->iterator_->first.type()->typeId();
				}
				else
				{
					return TypeId::getType<void>();
				}
			}

			static const TypeId& valueType(const this_type* impl)
			{
				if(impl->iterator_ != impl->container_.end())
				{
					return impl->iterator_->second.type()->typeId();
				}
				else
				{
					return TypeId::getType<void>();
				}
			}
		};

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

		template<bool can_set, typename Dummy = void>
		struct downcaster_impl
		{
			static bool downcast(container_type* v, CollectionImplBase& storage)
			{
				if( v && storage.containerData() == v )
				{
					// don't copy to itself
					return true;
				}

				if( storage.containerType() == TypeId::getType< container_type >() )
				{
					// short circuit
					auto container = static_cast< const container_type* >( storage.containerData() );
					if( v )
					{
						*v = *container;
					}
					return true;
				}

				if( v )
				{
					v->clear();
					for( auto it = storage.begin(), end = storage.end(); !it->equals( *end ); it->inc() )
					{
						key_type key;
						if( !it->key().tryCast( key ) )
						{
							return false;
						}

						value_type val;
						if( !it->value().tryCast( val ) )
						{
							return false;
						}

						v->emplace( std::move( key ), std::move( val ) );
					}
				}

				return true;
			}
		};

		template<typename Dummy>
		struct downcaster_impl<false, Dummy>
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


		const TypeId& keyType() const override
		{
			return TypeId::getType< key_type >();
		}


		const TypeId& valueType() const override
		{
			return TypeId::getType< value_type >();
		}


		const TypeId& containerType() const override
		{
			return TypeId::getType< container_type >();
		}


		void* containerData() const override
		{
			return ( void* )&container_;
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
					auto insertionPoint = container_.equal_range(k).second;
					auto r = container_.emplace_hint(insertionPoint, k, value_type());
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
						auto itr = range.first;
						decltype(itr) resultItr;

						while (itr != range.second)
						{
							resultItr = itr;
							++itr;
						}

						return result_type(
							std::make_shared< iterator_impl_type >(container_, resultItr), false);
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

		bool isMapping() const override
		{
			return true;
		}

		bool canResize() const override
		{
			return can_resize;
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


		const TypeId& keyType() const override
		{
			return TypeId::getType< key_type >();
		}


		const TypeId& valueType() const override
		{
			return TypeId::getType< value_type >();
		}


		const TypeId& containerType() const override
		{
			return TypeId::getType< container_type >();
		}


		void* containerData() const override
		{
			return ( void* )&container_;
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

		bool isMapping() const override
		{
			return true;
		}

		bool canResize() const override
		{
			return false;
		}

	private:
		container_type& container_;

	};


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

}

#endif

