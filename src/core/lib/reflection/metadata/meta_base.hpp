#ifndef METABASE_HPP
#define METABASE_HPP

/*
All reflected meta attribute classes should inherit from this class.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "../reflected_object.hpp"

class MetaBase
	: public ReflectedPolyStruct
{
	DECLARE_REFLECTED

public:
	MetaBase();
	virtual ~MetaBase();
protected:

public:
	ObjectHandle nextAsHandle() const;
	const MetaBase * next() const { return nextMetaData_; }
	void setNext( const MetaBase * next ) const { nextMetaData_ = next; }

	void setDefinitionName( const char * defName ) { definitionName_ = defName; }
	const char * getDefinitionName() const { return definitionName_; }
private:
	mutable const MetaBase * nextMetaData_;

	//Used for deferred loading
	const char *			 definitionName_;
};

const MetaBase & operator + ( const MetaBase & left, const MetaBase & right );

#endif