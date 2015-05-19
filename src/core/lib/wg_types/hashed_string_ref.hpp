#ifndef HASHED_STRING_REF_HPP
#define HASHED_STRING_REF_HPP

#include <functional>

class HashedStringRef
{
public:
	HashedStringRef( const char * str );

	//Getters
	size_t hash() const;

	//Operators
	bool operator == ( const HashedStringRef & other ) const;

private:
	size_t			hash_;
	const char *	pStart_;
	size_t			length_;
};

namespace std
{

	template<>
	struct hash< HashedStringRef >
		: public std::unary_function< const HashedStringRef, std::size_t >
	{
		std::size_t operator()(const HashedStringRef & s) const
		{
			return s.hash();
		}
	};

}

#endif // HASHED_STRING_REF_HPP