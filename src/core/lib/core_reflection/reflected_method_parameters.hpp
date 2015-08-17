#ifndef REFLECTED_METHOD_PARAMETERS_HPP
#define REFLECTED_METHOD_PARAMETERS_HPP

#include <vector>

class ObjectHandle;

class ReflectedMethodParameters
{
public:
	typedef ObjectHandle value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const ObjectHandle const_value_type;
	typedef const_value_type* const_pointer;
	typedef const_value_type& const_reference;

	ReflectedMethodParameters();
	ReflectedMethodParameters( const ObjectHandle& handle );

	bool empty() const;
	size_t size() const;

	void clear();

	void push_back( const ObjectHandle& handle );

	reference operator[]( size_t index );
	const_reference operator[]( size_t index ) const;

	struct const_iterator
	{
		friend class ReflectedMethodParameters;
		typedef std::random_access_iterator_tag iterator_category;
		typedef long long difference_type;

		const_reference operator*() const;
		const_iterator operator++( int );
		const_iterator& operator++();
		const_iterator operator+( difference_type n ) const;
		difference_type operator-( const const_iterator& rhs ) const;
		bool operator==( const const_iterator& rhs ) const;
		bool operator!=( const const_iterator& rhs ) const;
		bool operator<( const const_iterator& rhs ) const;

	protected:
		const_iterator( const ReflectedMethodParameters& collection, size_t index );

		ReflectedMethodParameters& collection_;
		size_t index_;
	};

	struct iterator: public const_iterator
	{
		friend class ReflectedMethodParameters;
		typedef std::random_access_iterator_tag iterator_category;
		typedef long long difference_type;

		reference operator*() const;
		iterator operator++( int );
		iterator& operator++();
		iterator operator+( difference_type n ) const;

	private:
		iterator( ReflectedMethodParameters& collection, size_t index );
	};

	const_iterator cbegin() const;
	const_iterator cend() const;
	iterator begin();
	iterator end();

private:
	std::vector<ObjectHandle> parameters_;
};

#endif //REFLECTED_METHOD_PARAMETERS_HPP
