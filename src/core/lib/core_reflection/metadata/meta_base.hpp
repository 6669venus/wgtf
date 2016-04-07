#ifndef METABASE_HPP
#define METABASE_HPP

/*
All reflected meta attribute classes should inherit from this class.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "../reflected_object.hpp"
#include "../object_handle.hpp"

#include "core_reflection/reflection_dll.hpp"

typedef ObjectHandleT< MetaBase > MetaHandle;

namespace MetaParamTypes
{
	enum MetaParamType
	{
		kUndefined,
		kBoolean,
		kInteger,
		kFloat,
		kString,
		kEnum,
		kHandle,
		kHandleList,
		kTable,
		kFunction,
		kImportName,
	};
}

#pragma warning (push)
#pragma warning (disable : 4251) // * needs to have dll-interface to be used by clients of class '*'

class REFLECTION_DLL MetaBase
{
	DECLARE_REFLECTED

public:
	MetaBase();
	virtual ~MetaBase();

private:
	MetaHandle next() const { return nextMetaData_; }
	void setNext( const MetaHandle & next ) const { nextMetaData_ = next; }

	mutable MetaHandle nextMetaData_;

	friend REFLECTION_DLL const MetaHandle & operator + ( const MetaHandle & left, const MetaHandle & right );
	friend REFLECTION_DLL MetaHandle findFirstMetaData( const TypeId & typeId, const MetaHandle & metaData, const IDefinitionManager & definitionManager );
};

#pragma warning (pop)

REFLECTION_DLL const MetaHandle & operator + ( const MetaHandle & left, const MetaHandle & right );

#endif