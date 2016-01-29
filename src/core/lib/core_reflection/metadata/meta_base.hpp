#ifndef METABASE_HPP
#define METABASE_HPP

/*
All reflected meta attribute classes should inherit from this class.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "../reflected_object.hpp"
#include "../object_handle.hpp"

typedef ObjectHandleT< MetaBase > MetaHandle;

class MetaBase
{
	DECLARE_REFLECTED

public:
	MetaBase();
	virtual ~MetaBase();

private:
	MetaHandle next() const { return nextMetaData_; }
	void setNext( const MetaHandle & next ) const { nextMetaData_ = next; }

	mutable MetaHandle nextMetaData_;

	friend const MetaHandle & operator + ( const MetaHandle & left, const MetaHandle & right );
	friend MetaHandle findFirstMetaData( const TypeId & typeId, const MetaHandle & metaData, const IDefinitionManager & definitionManager );
};

const MetaHandle & operator + ( const MetaHandle & left, const MetaHandle & right );

#endif