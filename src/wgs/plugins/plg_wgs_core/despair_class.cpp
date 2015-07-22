//
#include "reflection/function_property.hpp"
#include "reflection/interfaces/i_class_definition_details.hpp"
#include "reflection/interfaces/i_class_definition_modifier.hpp"
#include "reflection/utilities/definition_helpers.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "string_utils/string_utils.hpp"
#include <memory>

#include "game_common.h"
#include "despair_class.h"
#include "despair_property.h"
//
//--------------------------------------------------------------------------
DespairClassDefinition::DespairClassDefinition(const Despair::Reflection::ClassInfo * classInfo)
	: classInfo_(classInfo)
{
	// Convert and store the display name in wide characters
	const char* name = getName();

	std::wstring_convert< Utf16to8Facet > conversion(Utf16to8Facet::create());
	wideDisplayName_ = conversion.from_bytes(name);
	metaDisplayName_ = &MetaDisplayName(wideDisplayName_.c_str());
}

DespairClassDefinition::~DespairClassDefinition()
{
	delete metaDisplayName_;
}

//--------------------------------------------------------------------------
void DespairClassDefinition::init(IClassDefinitionModifier & modifier)
{
	modifier_ = &modifier;

	// Fields
	Despair::Reflection::ClassInfo::FieldList fields =
		classInfo_->GetFields();
	Despair::Reflection::ClassInfo::FieldList::const_iterator fieldItr =
		fields.begin();

	for (; fieldItr != fields.end(); ++fieldItr)
	{
		const Despair::Reflection::ClassField * field = (*fieldItr);
		if (field != nullptr)
		{
			auto fieldProperty = new DespairProperty(
				field->GetName(),
				TypeId(convertFieldTypeName(*field)), *field);
			modifier_->addProperty(fieldProperty,
				fieldProperty->getMetaDisplayName());
		}
	}
}

//--------------------------------------------------------------------------
bool DespairClassDefinition::isAbstract() const
{
	return false;
}

//--------------------------------------------------------------------------
bool DespairClassDefinition::isGeneric() const
{
	return false;
}

//--------------------------------------------------------------------------
const char * DespairClassDefinition::getName() const
{
	// Return the class name in a format consistent with other NGT
	// reflected objects.
	return classInfo_->GetName();
}

//--------------------------------------------------------------------------
const char * DespairClassDefinition::getParentName() const
{
	// Despair allows for multiple inheritance. Default to pulling the
	// first parent's name in this override.
	return getParentName(0);
}

//--------------------------------------------------------------------------
const char * DespairClassDefinition::getParentName(int index) const
{
	return classInfo_->GetBaseClass(index)->GetName();
}

//--------------------------------------------------------------------------
const MetaBase * DespairClassDefinition::getMetaData() const
{
	return metaDisplayName_;
}

//--------------------------------------------------------------------------
ObjectHandle DespairClassDefinition::createBaseProvider(const ReflectedPolyStruct & polyStruct) const
{
	auto pType = ReflectionUtilities::dynamicCast< GameObject >(polyStruct);
	return createBaseProvider(polyStruct.getDefinition(), pType);
}

//--------------------------------------------------------------------------
ObjectHandle DespairClassDefinition::createBaseProvider(const IClassDefinition & definition,const void * pThis) const
{
	/*return BaseProviderHelper<
		std::is_base_of< ReflectedPolyStruct, GameObject >::value
		>::getBaseProvider(pThis, &definition);*/

	// placeholder ... not sure how to replace the above line of code while not knowing all types at compile time.. :(
	return ObjectHandle();
}

//--------------------------------------------------------------------------
ObjectHandle DespairClassDefinition::create(const IClassDefinition & definition) const
{
	auto & pInst = std::unique_ptr< GameObject >(new GameObject());
	PolyStructDefinitionSetter setter(pInst.get(), &definition);
	return ObjectHandle(std::move(pInst), &definition);
}

//--------------------------------------------------------------------------
//TODO - this will be removed when ::create() is ready
/*ReflectedPolyStruct * createPolyStruct() const
{
return
createPolyStructHelper< Type >( static_cast< Type * >( nullptr ) );
}*/

//--------------------------------------------------------------------------
IClassDefinitionDetails::CastHelperCache * DespairClassDefinition::getCastHelperCache() const
{
	return &castHelperCache_;
}

//--------------------------------------------------------------------------
const char* DespairClassDefinition::convertFieldTypeName(const Despair::Reflection::ClassField & field) const
{
	// Some Despair types are defined in such a way that NGT will not
	// understand how to cast field values. This function will translate
	// those differences.

	switch (field.GetType())
	{
	case Despair::Reflection::ClassField::FIELDTYPE_INT:
	case Despair::Reflection::ClassField::FIELDTYPE_INT32:
	{
		return "int";
	}
	case Despair::Reflection::ClassField::FIELDTYPE_UINT:
	case Despair::Reflection::ClassField::FIELDTYPE_UINT32:
	{
		return "unsigned int";
	}
	case Despair::Reflection::ClassField::FIELDTYPE_STRING:
	{
		return "char";
	}
	case Despair::Reflection::ClassField::FIELDTYPE_VECTOR3D:
	{
		return TypeId::getType< Despair::Vector3d >().getName();
	}
	default:
	{
		//TODO - will need special handling for Despair::Vector3d
		return field.GetTypeName();
	}
	}

	return "";
}