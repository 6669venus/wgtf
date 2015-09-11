#ifndef I_CLASS_DEFINITION_DETAILS_HPP
#define I_CLASS_DEFINITION_DETAILS_HPP

#include <unordered_map>

class ObjectHandle;
class MetaBase;
class ReflectedPolyStruct;

class IClassDefinition;
class IClassDefinitionModifier;

class TypeId;

class IClassDefinitionDetails
{
public:
	typedef std::unordered_map< const TypeId, std::pair< bool, ptrdiff_t > > CastHelperCache;

	virtual ~IClassDefinitionDetails() {}
	virtual void init( IClassDefinitionModifier & ) = 0;
	virtual bool isAbstract() const = 0;
	virtual bool isGeneric() const = 0;
	virtual const char * getName() const = 0;
	virtual const char * getParentName() const = 0;
	virtual const MetaBase * getMetaData() const = 0;
	virtual ObjectHandle createBaseProvider( const ReflectedPolyStruct & ) const = 0;
	virtual ObjectHandle createBaseProvider(
		const IClassDefinition & classDefinition, const void * pThis ) const = 0;
	virtual ObjectHandle create(
		const IClassDefinition & classDefinition ) const = 0;
	virtual CastHelperCache * getCastHelperCache() const = 0;
	virtual void * upCast( void * object ) const = 0;
};

#endif // I_CLASS_DEFINITION_DETAILS_HPP

