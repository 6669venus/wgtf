#ifndef META_TYPES_HPP
#define META_TYPES_HPP

#include "meta_base.hpp"

#include "../types/reflected_collection.hpp"

class IEnumGenerator;

MetaBase & MetaNone();
MetaBase & MetaMinMax( float min, float max );
MetaBase & MetaStepSize( float stepSize );
MetaBase & MetaDecimals( int decimals );


#define MetaEnumFunc( getterFunc ) \
	MetaEnum(\
		new ReflectedCollectionImpl( \
			new FunctionProperty< std::map< int, std::wstring >,SelfType, true, true >(\
			"EnumTypes", &SelfType::getterFunc, NULL,\
			TypeId::getType< std::map< int, std::wstring > >() ) ) )

MetaBase & MetaEnum( IEnumGenerator * enumGenerator );
MetaBase & MetaEnum( const wchar_t * enumString );

MetaBase & MetaSlider();

MetaBase & MetaGroup( const wchar_t * groupName );

MetaBase & MetaAttributeDisplayName( const char * attributePath );

MetaBase & MetaDisplayName( const wchar_t * displayName );

MetaBase & MetaPanelLayout( const char * layoutFile, const char * bindingsFile = NULL );

MetaBase & MetaNoNull();

MetaBase & MetaColor();

MetaBase & MetaHidden();

MetaBase & MetaThumbnail( int width = -1, int height = -1 );

MetaBase & MetaInPlace( const char * propertyName = NULL );

MetaBase & MetaSelected( const char * propertyName );

MetaBase & MetaInPlacePropertyName( const char * propertyName );

MetaBase & MetaReadOnly();

MetaBase & MetaUrl(bool isAssetBrowserDialog = false,
				   const char * urlDlgTitle = nullptr, 
				   const char * urlDlgDefaultFolder = nullptr,
				   int urlDlgModality = 1,
				   const char * urlDlgNameFilters = nullptr, 
				   const char * urlDlgSelectedNameFilter = nullptr );

class IMetaCommandExecutable
{
public:
	virtual ~IMetaCommandExecutable() {}
	virtual void execute( void * pBase, const void * arguments ) const = 0;
};

template< typename SelfType >
class MetaCommandExecutableImpl
	: public IMetaCommandExecutable
{
public:
	typedef void (SelfType::*CommandExecuteFunc)( const void * );

	MetaCommandExecutableImpl( CommandExecuteFunc commandExecuteFunc )
		: commandExecuteFunc_( commandExecuteFunc )
	{
	}

	virtual void execute( void * pBase, const void * arguments ) const
	{
		typedef void (SelfType::*ActualFuncSignature)( const void * );
		ActualFuncSignature actualFunc = 
			reinterpret_cast< ActualFuncSignature >( commandExecuteFunc_ );
		( ( ( SelfType* ) pBase )->*actualFunc )( arguments );
	}

private:
	CommandExecuteFunc commandExecuteFunc_;
};

#define MetaCommand( commandName, executeFunc ) \
	MetaCommandBase( commandName,\
		new MetaCommandExecutableImpl< SelfType >(\
			reinterpret_cast< MetaCommandExecutableImpl< SelfType >::CommandExecuteFunc >(\
			&SelfType::executeFunc ) ) )

MetaBase & MetaCommandBase(
	const wchar_t * commandName, const IMetaCommandExecutable * executeFunc );

MetaBase & MetaNoSerialization();

MetaBase & MetaUniqueId( const char * id);

MetaBase & MetaOnStack();

#endif //META_TYPES_HPP