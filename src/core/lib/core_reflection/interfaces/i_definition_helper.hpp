#ifndef I_DEFINITION_HELPER_HPP
#define I_DEFINITION_HELPER_HPP

#include "core_variant/type_id.hpp"

class IClassDefinition;
class ObjectHandle;

class IDefinitionHelper
{
public:
	virtual ~IDefinitionHelper() {}

	virtual TypeId typeId() const = 0;

	virtual IClassDefinition * getDefinition( const ObjectHandle & object ) const = 0;
};

#endif