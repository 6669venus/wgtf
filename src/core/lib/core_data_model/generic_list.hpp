#ifndef GENERIC_LIST_HPP
#define GENERIC_LIST_HPP

#include "i_list_model.hpp"
#include "i_item.hpp"
#include "i_item_role.hpp"
#include "core_variant/variant.hpp"
#include <memory>

template< typename T >
class GenericListItemT : public IItem
{
public:
	GenericListItemT( const T& value )
		:value_( value )
	{
	}

	GenericListItemT( T&& value )
		: value_( value )
	{
	}

	virtual ~GenericListItemT()
	{
	}

	// IItem
	int columnCount() const override
	{
		return 1;
	}
	const char * getDisplayText( int column ) const override
	{
		return nullptr;
	}
	ThumbnailData getThumbnail( int column ) const override
	{
		return nullptr;
	}
	Variant getData( int column, size_t roleId ) const override
	{
		if (roleId == ValueTypeRole::roleId_)
		{
			return typeid( T ).name();
		}
		else if (roleId == ValueRole::roleId_)
		{
			return value_;
		}

		return Variant();
	}
	bool setData( int column, size_t roleId, const Variant & data ) override
	{
		if (roleId != ValueRole::roleId_)
		{
			return false;
		}
		T tmp;
		bool isOk = data.tryCast( tmp );
		if (!isOk)
		{
			return false;
		}
		value_ = tmp;
		return true;
	}
	//

	const T & value() const
	{
		return value_;
	}


	void value( const T & value )
	{
		value_ = value;
	}

	GenericListItemT & operator=( const T & data )
	{
		value_ = data;
		return *this;
	}
	GenericListItemT & operator=( T && data )
	{
		value_ = data;
		return *this;
	}
	bool operator==( const T & data ) const
	{
		return (value_ == data);
	}

private:
	T value_;
};

template < typename T >
class GenericListT
	: public IListModel
{
public:
	GenericListT() {}
	virtual ~GenericListT() {}

	// IListModel
	IItem * item( size_t index ) const override
	{
		if (index >= items_.size())
		{
			return nullptr;
		}

		return items_[index].get();
	}
	size_t index( const IItem * item ) const override
	{
		auto index = 0;
		auto it = items_.begin();
		for (; it != items_.end() && it->get() != item; ++index, ++it) {}
		assert( it != items_.end() );
		return index;
	}

	bool empty() const override
	{
		return items_.empty();
	}
	size_t size() const override
	{
		return items_.size();
	}

	bool canClear() const override
	{
		return true;
	}
	void clear() override
	{
		this->resize( 0 );
	}
	//

	typedef std::vector< std::unique_ptr< IItem > > Items;

	class ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::const_iterator::difference_type difference_type;
		typedef const GenericListItemT<T> value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		ConstIterator( const ConstIterator& rhs )
			: iterator_( new Items::const_iterator( *rhs.iterator_ ) )
		{
		}
		ConstIterator& operator=( const ConstIterator& rhs )
		{
			if (this != &rhs)
			{
				iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
			}
			return *this;
		}

		reference operator*( ) const
		{
			auto item = static_cast< value_type * >( (*iterator_)->get() );
			return *item;
		}
		ConstIterator & operator++( )
		{
			++(*iterator_);
			return *this;
		}
		ConstIterator operator++( int )
		{
			ConstIterator tmp = *this;
			++(*this);
			return tmp;
		}
		bool operator==( const ConstIterator & other ) const
		{
			return *iterator_ == *other.iterator_;
		}
		bool operator!=( const ConstIterator & other ) const
		{
			return !(*this == other);
		}
		bool operator<( const ConstIterator & other ) const
		{
			return *iterator_ < *other.iterator_;
		}
		difference_type operator-( const ConstIterator & other ) const
		{
			return *iterator_ - *other.iterator_;
		}
		ConstIterator operator+(difference_type n) const
		{
			return *iterator_ + n;
		}

	protected:
		ConstIterator() {}
		ConstIterator( const Items::const_iterator & iterator )
			: iterator_( new Items::const_iterator( iterator ) )
		{
		}

		const Items::const_iterator& iterator() const
		{
			return *iterator_;
		}

		std::unique_ptr<Items::const_iterator> iterator_;

		friend class GenericListT<T>;
	};

	class Iterator : public ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::iterator::difference_type difference_type;
		typedef GenericListItemT<T> value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		Iterator( const Iterator& rhs )
			: ConstIterator()
		{
			this->iterator_.reset( new Items::const_iterator( rhs.iterator() ) );
		}
		Iterator& operator=( const Iterator& rhs )
		{
			if (this != &rhs)
			{
				this->iterator_.reset( new Items::const_iterator( *rhs.iterator_ ) );
			}

			return *this;
		}

		reference operator*( ) const
		{
			auto item = static_cast< value_type * >( (*this->iterator_)->get() );
			return *item;
		}
		Iterator & operator++( )
		{
			++(*this->iterator_);
			return *this;
		}
		Iterator operator++( int )
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}
		Iterator operator+(difference_type n) const
		{
			*this->iterator_ += n;
			return *this;
		}

	private:
		Iterator( const Items::iterator & iterator )
			: ConstIterator()
		{
			this->iterator_.reset( new Items::const_iterator( iterator ) );
		}

		const Items::const_iterator& iterator() const
		{
			return *( this->iterator_.get() );
		}

		friend class GenericListT<T>;
	};

	void resize( size_t newSize )
	{
		auto oldSize = size();
		if (newSize < oldSize)
		{
			notifyPreItemsRemoved( nullptr, newSize, oldSize - newSize );
			items_.resize( newSize );
			notifyPostItemsRemoved( nullptr, newSize, oldSize - newSize );
		}
		else if (newSize > oldSize)
		{
			notifyPreItemsInserted( nullptr, oldSize, newSize - oldSize );
			items_.resize( newSize );
			notifyPostItemsInserted( nullptr, oldSize, newSize - oldSize );
		}
	}

	ConstIterator cbegin() const
	{
		return ConstIterator( items_.cbegin() );
	}
	ConstIterator cend() const
	{
		return ConstIterator( items_.cend() );
	}
	ConstIterator begin() const
	{
		return ConstIterator(items_.cbegin());
	}
	ConstIterator end() const
	{
		return ConstIterator(items_.cend());
	}
	Iterator begin()
	{
		return Iterator( items_.begin() );
	}
	Iterator end()
	{
		return Iterator( items_.end() );
	}

	Iterator insert( const Iterator & position, const T & value )
	{
		auto index = std::distance( items_.cbegin(), position.iterator() );

		notifyPreItemsInserted( nullptr, index, 1 );
		auto it = items_.emplace(
			position.iterator(), new GenericListItemT<T>( value ) );
		notifyPostItemsInserted( nullptr, index, 1 );

		return it;
	}
	Iterator erase( const Iterator & position )
	{
		auto index = std::distance( items_.cbegin(), position.iterator() );

		notifyPreItemsRemoved( nullptr, index, 1 );
		auto it = items_.erase( position.iterator() );
		notifyPostItemsRemoved( nullptr, index, 1 );

		return it;
	}
	Iterator erase( const Iterator & first, const Iterator & last )
	{
		auto index = std::distance( items_.cbegin(), first.iterator() );
		auto count = std::distance( first.iterator(), last.iterator() );

		notifyPreItemsRemoved( nullptr, index, count );
		auto it = items_.erase( first.iterator(), last.iterator() );
		notifyPostItemsRemoved( nullptr, index, count );

		return it;
	}

	void emplace_back( T && value )
	{
		const auto index = items_.size();

		notifyPreItemsInserted( nullptr, index, 1 );
		items_.emplace( items_.end(), new GenericListItemT<T>( std::move( value ) ) );
		notifyPostItemsInserted( nullptr, index, 1 );
	}
	
	void push_back( const T & value )
	{
		const auto index = items_.size();
		
		notifyPreItemsInserted( nullptr, index, 1 );
		items_.emplace( items_.end(), new GenericListItemT<T>( value ) );
		notifyPostItemsInserted( nullptr, index, 1 );
	}
	
	void push_front( const T & value )
	{
		auto index = 0;

		notifyPreItemsInserted( nullptr, index, 1 );
		items_.emplace( items_.begin(), new GenericListItemT<T>( value ) );
		notifyPostItemsInserted( nullptr, index, 1 );
	}

	T pop_back()
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.back().get() );
		T value = item->value_;

		auto index = items_.size() - 1;

		notifyPreItemsRemoved( nullptr, index, 1 );
		items_.pop_back();
		notifyPostItemsRemoved( nullptr, index, 1 );

		return value;
	}
	T pop_front()
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.front().get() );
		T value = item->value_;

		auto index = 0;

		notifyPreItemsRemoved( nullptr, index, 1 );
		items_.erase( items_.begin() );
		notifyPostItemsRemoved( nullptr, index, 1 );

		return value;
	}
	const T & back() const
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.back().get() );
		const T & value = item->value_;

		return value;
	}
	const T & front() const
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_.front().get() );
		const T & value = item->value_;

		return value;
	}

	GenericListItemT<T> & operator[](size_t index)
	{
		auto item = static_cast< GenericListItemT<T> * >( items_[index].get() );
		return *item;
	}
	const GenericListItemT<T> & operator[](size_t index) const
	{
		auto item = static_cast< const GenericListItemT<T> * >( items_[index].get() );
		return *item;
	}

	// temporary add these interfaces for fixing
	// bug http://jira.bigworldtech.com/browse/NGT-387
	// TODO: we may re-factor these in the future
	void emplace_back( GenericListItemT<T> * item );
	void push_back( GenericListItemT<T> * item );
	void push_front( GenericListItemT<T> * item );

private:
	GenericListT( const GenericListT& rhs );
	GenericListT& operator=( const GenericListT& rhs );

	Items items_;

	friend class Iterator;
};



///////////////////////////////////////////////////////////////////////////////
// deprecated class
class 
#ifdef _WIN32
	__declspec(deprecated("GenericList is deprecated, please use VariantList instead"))
#elif __APPLE__
	__attribute__((deprecated("GenericList is deprecated, please use VariantList instead")))
#endif
GenericListItem : public IItem
{
public:
	GenericListItem( const Variant& value );
	GenericListItem( Variant&& value );
	virtual ~GenericListItem()
	{

	}
	// IItem
	int columnCount() const override;
	const char * getDisplayText( int column ) const override;
	ThumbnailData getThumbnail( int column ) const override;
	Variant getData( int column, size_t roleId ) const override ;
	bool setData( int column, size_t roleId, const Variant & data ) override;
	//

	template<typename T>
	T value() const
	{
		return value_.value< T >();
	}

#ifdef _WIN32
	template<>
	const Variant & value<const Variant &>() const
	{
		return value_;
	}
#endif // _WIN32

	template<typename T>
	void value( const T & value )
	{
		value_ = value;
	}

	GenericListItem & operator=( const Variant & data );
	GenericListItem & operator=( Variant && data );
	bool operator==( const Variant & data ) const;

private:
	Variant value_;

	friend class GenericList;
};

#ifdef __APPLE__
	template<>
	const Variant & GenericListItem::value<const Variant &>() const;
#endif // __APPLE__

class
#ifdef _WIN32
	__declspec(deprecated("GenericList is deprecated, please use VariantList instead"))
#elif __APPLE__
	__attribute__((deprecated("GenericList is deprecated, please use VariantList instead")))
#endif
	GenericList
	: public IListModel
{
public:
	GenericList();
	virtual ~GenericList();

	// IListModel
	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem * item ) const override;

	virtual bool empty() const override;
	virtual size_t size() const override;

	bool canClear() const override;
	void clear() override;
	//

	typedef std::vector< std::unique_ptr< IItem > > Items;

	class ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::const_iterator::difference_type difference_type;
		typedef const GenericListItem value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		ConstIterator( const ConstIterator& rhs );
		ConstIterator& operator=( const ConstIterator& rhs );

		reference operator*( ) const;
		ConstIterator & operator++( );
		ConstIterator operator++( int );
		bool operator==( const ConstIterator & other ) const;
		bool operator!=( const ConstIterator & other ) const;
		bool operator<( const ConstIterator & other ) const;
		difference_type operator-( const ConstIterator & other ) const;
		ConstIterator operator+(difference_type n) const;

	protected:
		ConstIterator() {}
		ConstIterator( const Items::const_iterator & iterator );

		const Items::const_iterator& iterator() const;

		std::unique_ptr<Items::const_iterator> iterator_;

		friend class GenericList;
	};

	class Iterator : public ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::iterator::difference_type difference_type;
		typedef GenericListItem value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		Iterator( const Iterator& rhs );
		Iterator& operator=( const Iterator& rhs );

		reference operator*( ) const;
		Iterator & operator++( );
		Iterator operator++( int );
		Iterator operator+(difference_type n) const;

	private:
		Iterator( const Items::iterator & iterator );

		const Items::const_iterator& iterator() const;

		friend class GenericList;
	};

	void resize( size_t newSize );

	ConstIterator cbegin() const;
	ConstIterator cend() const;
	ConstIterator begin() const;
	ConstIterator end() const;
	Iterator begin();
	Iterator end();

	Iterator insert( const Iterator & position, const Variant & value );
	Iterator erase( const Iterator & position );
	Iterator erase( const Iterator & first, const Iterator & last );

	void emplace_back( Variant && value );
	void push_back( Variant && value );
	void push_back( const Variant & value );
	void push_front( const Variant & value );
	Variant pop_back();
	Variant pop_front();
	const Variant & back() const;
	const Variant & front() const;

	GenericListItem & operator[](size_t index);
	const GenericListItem & operator[](size_t index) const;

	// temporary add these interfaces for fixing
	// bug http://jira.bigworldtech.com/browse/NGT-387
	// TODO: we may re-factor these in the future
	void emplace_back( GenericListItem * item );
	void push_back( GenericListItem * item );
	void push_front( GenericListItem * item );

private:
	GenericList( const GenericList& rhs );
	GenericList& operator=( const GenericList& rhs );

	Items items_;

	friend class Iterator;
};

#endif // GENERIC_LIST_HPP
