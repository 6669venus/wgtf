#ifndef REF_OBJECT_ID_HPP
#define REF_OBJECT_ID_HPP

#include <string>
#include "wg_types/hash_utilities.hpp"

class RefObjectId
{
private:
	// NOTE: Structure must be compatible with Microsoft's GUID structure since
	// we cast to GUID in one of functions.
	unsigned int a_;
	unsigned int b_;
	unsigned int c_;
	unsigned int d_;

	static RefObjectId s_zero_;

public:
	RefObjectId();
	RefObjectId( const RefObjectId& other );
	RefObjectId( const std::string & s );
	RefObjectId( unsigned int a, unsigned int b, unsigned int c, unsigned int d ) : 
		a_( a ), b_( b ), c_( c ), d_( d )
	{}

	unsigned int getA() const	{ return a_; }
	unsigned int getB() const	{ return b_; }
	unsigned int getC() const	{ return c_; }
	unsigned int getD() const	{ return d_; }

	std::string toString() const {return *this;}
	operator std::string() const;

	std::wstring toWString() const;

	bool operator==( const RefObjectId & rhs ) const;
	bool operator!=( const RefObjectId & rhs ) const;
	bool operator<( const RefObjectId & rhs ) const;

	static RefObjectId generate();
	static const RefObjectId & zero() { return s_zero_; }

private:
    static bool fromString( const std::string & s, unsigned int * data );
};

namespace std
{
	template<>
	struct hash< const RefObjectId >
		: public std::unary_function< const RefObjectId, uint64_t>
	{
		uint64_t operator()( const RefObjectId & v ) const
		{
			uint64_t seed = 0;
			HashUtilities::combine( seed, v.getA() );
			HashUtilities::combine( seed, v.getB() );
			HashUtilities::combine( seed, v.getC() );
			HashUtilities::combine( seed, v.getD() );
			return seed;
		}
	};
}

#endif // REF_OBJECT_ID_HPP
