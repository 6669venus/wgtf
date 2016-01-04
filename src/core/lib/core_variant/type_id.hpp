#ifndef GLOBAL_TYPE_ID_HPP
#define GLOBAL_TYPE_ID_HPP

#include <stdint.h>
#include <functional>
#include <typeinfo>

class TypeId
{
private:
	TypeId( const std::string & name );

public:
	TypeId( const char * name );
	TypeId( const char * name, uint64_t hashCode );

	~TypeId();

	uint64_t getHashcode() const { return hashCode_; }
	const char * getName() const { return name_; }

	bool isPointer() const;
	TypeId removePointer() const;

	bool operator == ( const TypeId & other ) const;
	bool operator != ( const TypeId & other ) const;
	bool operator < (const TypeId & other) const;

	//==========================================================================
	template< typename T >
	static const TypeId& getType()
	{
		static const TypeId s_typeId( typeid( T ).name() );
		return s_typeId;
	}

private:
	const char *	name_;
	uint64_t		hashCode_;
	bool			owns_;
};

namespace std
{
	template<>
	struct hash< const TypeId >
		: public unary_function< const TypeId, size_t >
	{
	public:
		size_t operator()( const TypeId & v ) const
		{
			hash< uint64_t > hash_fn;
			return hash_fn( v.getHashcode() );
		}
	};
}

#endif //GLOBAL_TYPE_ID_HPP