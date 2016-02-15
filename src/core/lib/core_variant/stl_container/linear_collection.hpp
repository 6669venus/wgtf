#ifndef LINEAR_CONTAINER_COLLECTION_HPP
#define LINEAR_CONTAINER_COLLECTION_HPP

#include <vector>
#include <deque>
#include <array>
#include <cassert>
#include <utility>


namespace collection_details
{

	template<typename Container>
	class LinearCollectionIteratorImpl:
		public CollectionIteratorImplBase
	{
	public:
		typedef Container container_type;
		typedef typename container_type::size_type key_type;
		typedef typename container_type::value_type value_type;
		typedef LinearCollectionIteratorImpl<container_type> this_type;

		static const bool can_set =
			Variant::traits<value_type>::can_downcast &&
			!std::is_const<container_type>::value &&
			!std::is_const<value_type>::value;

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

		const TypeId& keyType() const override
		{
			return TypeId::getType<key_type>();
		}

		const TypeId& valueType() const override
		{
			return GetTypeImpl<value_type>::valueType(this);
		}

		Variant key() const override
		{
			return index_;
		}

		Variant value() const override
		{
			if(index_ < container_.size())
			{
				return GetImpl<value_type>::value(this);
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

		// SetImpl
		template<bool can_set, typename Dummy = void>
		struct SetImpl
		{
			static bool setValue(const this_type* impl, const Variant& v)
			{
				if(impl->index_ >= impl->container_.size())
				{
					return false;
				}

				return v.with<value_type>([impl](const value_type& val)
				{
					impl->container_[impl->index_] = val;
				});
			}
		};

		template<typename Dummy>
		struct SetImpl<false, Dummy>
		{
			static bool setValue(const this_type*, const Variant&)
			{
				// nop
				return false;
			}
		};

		// GetImpl
		template<typename T, typename Dummy = void>
		struct GetImpl
		{
			static Variant value(const this_type* impl)
			{
				return impl->container_[impl->index_];
			}
		};

		template<typename Dummy>
		struct GetImpl<bool, Dummy>
		{
			static Variant value(const this_type* impl)
			{
				return (bool)impl->container_[impl->index_];
			}
		};

		// GetTypeImpl
		template<typename T, typename Dummy = void>
		struct GetTypeImpl
		{
			static const TypeId& valueType(const this_type*)
			{
				return TypeId::getType<T>();
			}
		};

		template<typename Dummy>
		struct GetTypeImpl<Variant, Dummy>
		{
			static const TypeId& valueType(const this_type* impl)
			{
				if(impl->index_ < impl->container_.size())
				{
					return impl->container_[impl->index_].type()->typeId();
				}
				else
				{
					return TypeId::getType<void>();
				}
			}
		};

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

		template<bool can_set, typename Dummy = void>
		struct downcaster_impl
		{
			static bool downcast( container_type* v, CollectionImplBase& storage )
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
					v->reserve( storage.size() );
					for( auto it = storage.begin(), end = storage.end(); !it->equals( *end ); it->inc() )
					{
						key_type i;
						if( !it->key().tryCast( i ) )
						{
							return false;
						}

						value_type val;
						if( !it->value().tryCast( val ) )
						{
							return false;
						}

						if( i >= v->size() )
						{
							// WARNING! Can potentially cause OOM issues.
							v->resize( i + 1 );
						}

						(*v)[i] = std::move( val );
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

		explicit LinearCollectionImpl(container_type& container):
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

		bool isMapping() const override
		{
			return false;
		}

		bool canResize() const override
		{
			return can_resize;
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
					v->fill( value_type() );
					for( auto it = storage.begin(), end = storage.end(); !it->equals( *end ); it->inc() )
					{
						key_type i;
						if( !it->key().tryCast( i ) )
						{
							return false;
						}

						if( i >= v->size() )
						{
							return false;
						}

						value_type val;
						if( !it->value().tryCast(val) )
						{
							return false;
						}

						(*v)[i] = std::move( val );
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

		explicit LinearCollectionImpl(container_type& container):
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

		bool isMapping() const override
		{
			return false;
		}

		bool canResize() const override
		{
			return false;
		}

	private:
		container_type& container_;

	};


	// std::vector

	template<typename T, typename Alloc>
	LinearCollectionImpl<std::vector<T, Alloc>, true> deduceCollectionImplType(std::vector<T, Alloc>&);

	template<typename T, typename Alloc>
	LinearCollectionImpl<const std::vector<T, Alloc>, false> deduceCollectionImplType(const std::vector<T, Alloc>&);

	// std::deque

	template<typename T, typename Alloc>
	LinearCollectionImpl<std::deque<T, Alloc>, true> deduceCollectionImplType(std::deque<T, Alloc>&);

	template<typename T, typename Alloc>
	LinearCollectionImpl<const std::deque<T, Alloc>, false> deduceCollectionImplType(const std::deque<T, Alloc>&);

	// std::array

	template<typename T, size_t N>
	LinearCollectionImpl<std::array<T, N>, false> deduceCollectionImplType(std::array<T, N>&);

	template<typename T, size_t N>
	LinearCollectionImpl<const std::array<T, N>, false> deduceCollectionImplType(const std::array<T, N>&);

}

#endif

