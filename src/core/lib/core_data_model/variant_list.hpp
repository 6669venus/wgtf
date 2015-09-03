#ifndef VARIANT_LIST_HPP
#define VARIANT_LIST_HPP

#include "i_list_model.hpp"
#include "i_item.hpp"
#include <memory>

class VariantListItem : public IItem
{
public:
	VariantListItem( const Variant& value );
	VariantListItem( Variant&& value );
	virtual ~VariantListItem()
	{

	}
	// IItem
	int columnCount() const override;
	const char * getDisplayText( int column ) const override;
	ThumbnailData getThumbnail( int column ) const override;
	Variant getData( int column, size_t roleId ) const;
	bool setData( int column, size_t roleId, const Variant & data );
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

	VariantListItem & operator=( const Variant & data );
	VariantListItem & operator=( Variant && data );
	bool operator==( const Variant & data ) const;

private:
	Variant value_;

	friend class VariantList;
};

#ifdef __APPLE__
	template<>
	const Variant & VariantListItem::value<const Variant &>() const;
#endif // __APPLE__

class VariantList
	: public IListModel
{
public:
	VariantList();
	virtual ~VariantList();

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
		typedef const VariantListItem value_type;
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

		friend class VariantList;
	};

	class Iterator : public ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::iterator::difference_type difference_type;
		typedef VariantListItem value_type;
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

		friend class VariantList;
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
	void push_back( const Variant & value );
	void push_front( const Variant & value );
	Variant pop_back();
	Variant pop_front();
	const Variant & back() const;
	const Variant & front() const;

	VariantListItem & operator[](size_t index);
	const VariantListItem & operator[](size_t index) const;

	// temporary add these interfaces for fixing
	// bug http://jira.bigworldtech.com/browse/NGT-387
	// TODO: we may re-factor these in the future
	void emplace_back( VariantListItem * item );
	void push_back( VariantListItem * item );
	void push_front( VariantListItem * item );

private:
	VariantList( const VariantList& rhs );
	VariantList& operator=( const VariantList& rhs );

	Items items_;

	friend class Iterator;
};

#endif // VARIANT_LIST_HPP
