#include "pch.hpp"

#include "reflection/class_definition.hpp"
#include "reflection/reflected_property.hpp"
#include "reflection/function_property.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/definition_manager.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"
#include "reflection/utilities/reflection_variable_utilities.hpp"

#include "test_objects.hpp"

namespace wgt
{
BW_BEGIN_NAMESPACE

REFLECTED_BEGIN_EXPOSE(TestDefinitionObject, ReflectedObject, MetaNone())
	EXPOSE(counter_, "counter", MetaNone())
	EXPOSE(text_, "text", MetaNone())
	EXPOSE(values_, "values", MetaNone())
	EXPOSE("functional counter", getCounter, setCounter, MetaNone())
	EXPOSE("functional text", getText, setText, MetaNone())
	EXPOSE("counter getter", getCounter, MetaNone())
	EXPOSE("text getter", getText, MetaNone())
	EXPOSE(position_, "position", MetaNone())
REFLECTED_END_EXPOSE()


class InitReflectedTypes
{
public:
	InitReflectedTypes() :
		objectManager_(),
		definitionManager_( objectManager_ )
	{
		objectManager_.init( & definitionManager_ );
		IDefinitionManager & definitionManager = definitionManager_;
		REGISTER_DEFINITION( TestDefinitionObject );
	}
private:
	ObjectManager objectManager_;
	DefinitionManager definitionManager_;

};
static InitReflectedTypes s_refTypesInitializer;

BW_END_NAMESPACE

}
