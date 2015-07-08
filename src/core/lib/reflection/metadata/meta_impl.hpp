#ifndef META_IMPL_HPP
#define META_IMPL_HPP

/*
Standard meta attribute classes for the reflected system.
Details: https://confluence.wargaming.net/display/NGT/NGT+Reflection+System
*/

#include "meta_base.hpp"

class Collection;

//==============================================================================
class MetaNoneObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaNoneObj() {}
	~MetaNoneObj() {}
};

//==============================================================================
class MetaMinMaxObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaMinMaxObj(){}
	MetaMinMaxObj( float min, float max );

	const float & getMin() const { return min_; }
	const float & getMax() const { return max_; }

private:
	float min_;
	float max_;
};


class IEnumGenerator;
//==============================================================================
class MetaEnumObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaEnumObj()
		: enumGenerator_( NULL )
		, enumString_( NULL )
	{
	}

	explicit MetaEnumObj( IEnumGenerator * enumGenerator )
		: enumGenerator_( enumGenerator )
		, enumString_( NULL )
	{
	}

	explicit MetaEnumObj( const wchar_t * enumString )
		: enumGenerator_( NULL )
		, enumString_( enumString )
	{
	}

	~MetaEnumObj();

	const wchar_t * getEnumString() const;

	Collection generateEnum( const ObjectHandle & provider ) const;

private:
	IEnumGenerator *	enumGenerator_;
	const wchar_t *		enumString_;
};


//==============================================================================
class MetaSliderObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaSliderObj() {}
	~MetaSliderObj() {}
};


//==============================================================================
class MetaGroupObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaGroupObj()
		: groupName_( NULL )
	{
	}

	explicit MetaGroupObj( const wchar_t * groupName );
	~MetaGroupObj() {}

	const wchar_t * getGroupName() const;

private:
	const wchar_t * groupName_;
};


//==============================================================================
class MetaAttributeDisplayNameObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaAttributeDisplayNameObj()
		: attributeName_( NULL )
	{
	}

	explicit MetaAttributeDisplayNameObj( const char * attributeName );
	~MetaAttributeDisplayNameObj() {}

	const char * getAttributeName() const;

private:
	const char * attributeName_;
};


//==============================================================================
class MetaDisplayNameObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaDisplayNameObj()
		: displayName_( NULL )
	{
	}

	explicit MetaDisplayNameObj( const wchar_t * displayName );
	~MetaDisplayNameObj() {}

	const wchar_t * getDisplayName() const;

private:
	const wchar_t * displayName_;
};


//==============================================================================
class MetaPanelLayoutObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	explicit MetaPanelLayoutObj( 
		const char * layoutFile = NULL, 
		const char * bindingsFile = NULL) :
		layoutFile_(layoutFile),
		bindingsFile_(bindingsFile_)
	{
	}

	~MetaPanelLayoutObj() {}

	const char * getLayoutFile() const { return layoutFile_; }
	const char * getBindingsFile() const { return bindingsFile_; }

private:
	const char * layoutFile_;
	const char * bindingsFile_;
};


//==============================================================================
class MetaNoNullObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaNoNullObj() {}
	~MetaNoNullObj() {}
};


//==============================================================================
class MetaColorObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaColorObj() {}
	~MetaColorObj() {}
};


//==============================================================================
class MetaHiddenObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaHiddenObj() {}
	~MetaHiddenObj() {}
};


//==============================================================================
class MetaThumbnailObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaThumbnailObj( int width = -1, int height = -1 )
		: width_( width )
		, height_( height )
	{
	}

	~MetaThumbnailObj() {}

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }

private:
	int width_;
	int height_;
};


//==============================================================================
class MetaInPlaceObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	explicit MetaInPlaceObj( const char * propName = NULL )
		: propName_( propName )
	{
	}

	const char * getPropName() const { return propName_; }

private:
	const char * propName_;
};


//==============================================================================
class MetaSelectedObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	explicit MetaSelectedObj( const char * propName = NULL )
		: propName_( propName )
	{
	}

	const char * getPropName() const { return propName_; }

private:
	const char * propName_;
};


class IMetaCommandExecutable;

//==============================================================================
class MetaCommandObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaCommandObj();

	explicit MetaCommandObj(
		const wchar_t * commandName,
		const IMetaCommandExecutable * commandExecutable )
		: commandName_( commandName )
		, commandExecutable_( commandExecutable )
	{
	}

	~MetaCommandObj();

	const wchar_t * getCommandName() const { return commandName_; }

	void execute( void * pBase, void * arguments ) const;

private:
	const wchar_t *					commandName_;
	const IMetaCommandExecutable *	commandExecutable_;
};

//==============================================================================
class MetaNoSerializationObj
	: public MetaBase
{
	DECLARE_REFLECTED
	
public:
	MetaNoSerializationObj() {}
	~MetaNoSerializationObj() {}
};

//==============================================================================
class MetaUniqueIdObj
	: public MetaBase
{
	DECLARE_REFLECTED
public:
	MetaUniqueIdObj( const char * id = nullptr )
		: id_( id )
	{
	}
	~MetaUniqueIdObj() {}

	const char * getId() const;
private:
	const char * id_;
};

//==============================================================================
class MetaOnStackObj
	: public MetaBase
{
	DECLARE_REFLECTED

public:
	MetaOnStackObj() {}
	~MetaOnStackObj() {}
};

#endif //META_IMPL_HPP