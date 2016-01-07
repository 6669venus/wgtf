#ifndef METABASE_HPP
#define METABASE_HPP

/*
All reflected meta attribute classes should inherit from this class.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "../reflected_object.hpp"
#include "../object_handle.hpp"

class MetaBase
{
	DECLARE_REFLECTED

public:
	MetaBase();
	virtual ~MetaBase();

	void initialise( const TypeId & typeId, const ObjectHandleT< MetaBase > & handle );

	TypeId typeId() const { return typeId_; }
	ObjectHandleT< MetaBase > handle() const { return handle_; }

	const MetaBase * next() const { return nextMetaData_; }
	void setNext( const MetaBase * next ) const { nextMetaData_ = next; }

private:
	TypeId typeId_;
	ObjectHandleT< MetaBase > handle_;

	mutable const MetaBase * nextMetaData_;
};

const MetaBase & operator + ( const MetaBase & left, const MetaBase & right );

#endif