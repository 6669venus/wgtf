
#include "string_utils/string_utils.hpp"
#include "reflection/function_property.hpp"
#include "reflection/interfaces/i_class_definition_details.hpp"
#include "reflection/interfaces/i_class_definition_modifier.hpp"
#include "reflection/utilities/definition_helpers.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "reflection/metadata/meta_types.hpp"

#include "game_common.h"
#include "game_loader.h"
#include "despair_property.h"



DespairProperty::DespairProperty(const char * name, const TypeId & type, const Despair::Reflection::ClassField & field)
	: BaseProperty(name, type)
	, field_(field)
{
	std::wstring_convert< Utf16to8Facet > conversion(Utf16to8Facet::create());
	wideDisplayName_ = conversion.from_bytes(field.GetName());
	metaDisplayName_ = &MetaDisplayName(wideDisplayName_.c_str());
}

const MetaBase * DespairProperty::getMetaDisplayName() const
{
	return metaDisplayName_;
}

bool DespairProperty::set(const ObjectHandle & handle, const Variant & value) const
{
	return GameBinaryLoader::GetInstance()->setValue(handle, value, field_); 
}

Variant DespairProperty::get(const ObjectHandle & handle) const
{
	return GameBinaryLoader::GetInstance()->getValue(handle, field_); 
}
