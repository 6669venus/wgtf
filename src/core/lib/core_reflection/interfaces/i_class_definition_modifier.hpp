#ifndef I_CLASS_DEFINITION_MODIFIER_HPP
#define I_CLASS_DEFINITION_MODIFIER_HPP

class IBaseProperty;
class IDefinitionManager;
class MetaBase;

class IClassDefinitionModifier
{
public:
	virtual ~IClassDefinitionModifier() {}
	virtual void addProperty(
		IBaseProperty * reflectedProperty, const MetaBase * metaBase ) = 0;
	virtual size_t getPropertyCount() const = 0;
	virtual void setDefinitionManager( IDefinitionManager * defManager ) = 0;
};

#endif // I_CLASS_DEFINITION_MODIFIER_HPP

