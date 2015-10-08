#include "reflected_types.hpp"

#include "generic/generic_object.hpp"
#include "reflected_object.mpp" 

#include "i_definition_manager.hpp"

#include "metadata/meta_impl.hpp"

namespace Reflection
{

void initReflectedTypes( IDefinitionManager & definitionManager )
{
	REGISTER_DEFINITION( MetaNoneObj );
	REGISTER_DEFINITION( MetaBase );
	REGISTER_DEFINITION( MetaMinMaxObj );
	REGISTER_DEFINITION( MetaEnumObj );
	REGISTER_DEFINITION( MetaSliderObj );
	REGISTER_DEFINITION( MetaAttributeDisplayNameObj );
	REGISTER_DEFINITION( MetaGroupObj );
	REGISTER_DEFINITION( MetaDisplayNameObj );
	REGISTER_DEFINITION( MetaNoNullObj );
	REGISTER_DEFINITION( MetaColorObj );
	REGISTER_DEFINITION( MetaThumbnailObj );
	REGISTER_DEFINITION( MetaInPlaceObj );
	REGISTER_DEFINITION( MetaSelectedObj );
	REGISTER_DEFINITION( MetaHiddenObj );
	REGISTER_DEFINITION( MetaPanelLayoutObj );
	REGISTER_DEFINITION( MetaCommandObj );
	REGISTER_DEFINITION( MetaNoSerializationObj );
	REGISTER_DEFINITION( MetaUniqueIdObj );
	REGISTER_DEFINITION( MetaOnStackObj );
	REGISTER_DEFINITION( MetaInPlacePropertyNameObj );
	REGISTER_DEFINITION( MetaReadOnlyObj );
	
	REGISTER_DEFINITION( ReflectedPolyStruct );
	REGISTER_DEFINITION( GenericObject );
}

}