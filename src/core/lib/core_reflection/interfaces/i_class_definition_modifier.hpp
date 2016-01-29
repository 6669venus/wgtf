#ifndef I_CLASS_DEFINITION_MODIFIER_HPP
#define I_CLASS_DEFINITION_MODIFIER_HPP

#include <memory>

class IBaseProperty;
class IDefinitionManager;
class MetaBase;

typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;

class IClassDefinitionModifier
{
public:
	virtual ~IClassDefinitionModifier() {}
	virtual void addProperty( const IBasePropertyPtr & reflectedProperty, MetaHandle metaData ) = 0;
};

#endif // I_CLASS_DEFINITION_MODIFIER_HPP

