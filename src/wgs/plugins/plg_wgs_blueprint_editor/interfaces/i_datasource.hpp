#ifndef I_DATASOURCE_HPP
#define I_DATASOURCE_HPP

#include "reflection/reflected_object.hpp"
#include "reflection/generic/generic_object.hpp"
#include "generic_plugin/interfaces/i_component_context.hpp"

class BlueprintPage;

class IDataSource
{
public:
	virtual void init( IComponentContext & contextManager ) = 0;
	virtual void fini( IComponentContext & contextManager ) = 0;
	virtual const ObjectHandleT< BlueprintPage > & getBlueprintPage() const = 0;
};

#endif // I_DATASOURCE_HPP