#include "blueprint_model.hpp"
#include "interfaces/i_datasource.hpp"

#include "reflection/i_object_manager.hpp"

BlueprintStringProperty::BlueprintStringProperty()
	: stringValue_(L"TestText")
{
}

BlueprintStringProperty::~BlueprintStringProperty()
{
}

void BlueprintStringProperty::init()
{
}

void BlueprintStringProperty::setValueField(const std::wstring & text)
{
	stringValue_ = text;
}
void BlueprintStringProperty::getValueField(std::wstring * text) const
{
	*text = stringValue_;
}



BlueprintBooleanProperty::BlueprintBooleanProperty()
	: boolValue_(false)
{
}

BlueprintBooleanProperty::~BlueprintBooleanProperty()
{
}

void BlueprintBooleanProperty::init()
{
}

void BlueprintBooleanProperty::setValueField(const bool & checked)
{
	boolValue_ = checked;
}
void BlueprintBooleanProperty::getValueField(bool * checked) const
{
	*checked = boolValue_;
}



BlueprintFloatProperty::BlueprintFloatProperty()
	: floatValue_(0.0f)
{
}

BlueprintFloatProperty::~BlueprintFloatProperty()
{
}

void BlueprintFloatProperty::init()
{
}

void BlueprintFloatProperty::setValueField(const float & num)
{
	floatValue_ = num;
}
void BlueprintFloatProperty::getValueField(float * num) const
{
	*num = floatValue_;
}



BlueprintBaseProperty::BlueprintBaseProperty()
	: stringName_(L"TestText")
	, stringDisplayName_(L"TestText")
	, stringDescription_(L"TestText")
{
}

BlueprintBaseProperty::~BlueprintBaseProperty()
{
}

void BlueprintBaseProperty::init()
{
}

void BlueprintBaseProperty::setNameField(const std::wstring & text)
{
	stringName_ = text;
}
void BlueprintBaseProperty::getNameField(std::wstring * text) const
{
	*text = stringName_;
}

void BlueprintBaseProperty::setDisplayNameField(const std::wstring & text)
{
	stringDisplayName_ = text;
}
void BlueprintBaseProperty::getDisplayNameField(std::wstring * text) const
{
	*text = stringDisplayName_;
}

void BlueprintBaseProperty::setDescriptionField(const std::wstring & text)
{
	stringDescription_ = text;
}
void BlueprintBaseProperty::getDescriptionField(std::wstring * text) const
{
	*text = stringDescription_;
}



BlueprintPropertyContainer::BlueprintPropertyContainer()
	: stringName_(L"TestText")
{
	BlueprintFloatProperty floatProperty1;
	floatProperty1.setNameField(L"fMaxHitPoints");
	floatProperty1.setValueField(200.0f);
	floatProperty1.setDisplayNameField(L"Max HPs");
	floatProperty1.setDescriptionField(L"The speed of the vehicle");
	BlueprintFloatProperty floatProperty2;
	floatProperty2.setNameField(L"InitialHitPointPercent");
	floatProperty2.setValueField(1.5f);
	floatProperty2.setDisplayNameField(L"Initial Percent Hit Points");
	floatProperty2.setDescriptionField(L"The turning speed of the vehicle");
	BlueprintFloatProperty floatProperty3;
	floatProperty3.setNameField(L"MinTimeMustExistBeforeRegisteringToTakeDamage");
	floatProperty3.setValueField(0.4f);
	floatProperty3.setDisplayNameField(L"Minimum Time Must Exist Before Registering To Take Damage");
	floatProperty3.setDescriptionField(L"The time (in seconds) this object must exist before it can take any damage. This basically makes the object invulnerable until this amount of time has passed.");
	BlueprintBooleanProperty boolProperty1;
	boolProperty1.setNameField(L"CanReachZeroHP");
	boolProperty1.setValueField(true);
	boolProperty1.setDisplayNameField(L"Can Reach Zero HPs");
	boolProperty1.setDescriptionField(L"Can Reach Zero HPs");
	BlueprintBooleanProperty boolProperty2;
	boolProperty2.setNameField(L"ReloadWithInitialHP");
	boolProperty2.setValueField(true);
	boolProperty2.setDisplayNameField(L"Can Reload with Initial HP");
	boolProperty2.setDescriptionField(L"");
	BlueprintBooleanProperty boolProperty3;
	boolProperty3.setNameField(L"AllowEnemyDirectDamage");
	boolProperty3.setValueField(false);
	boolProperty3.setDisplayNameField(L"Allow Enemy Direct Damage");
	boolProperty3.setDescriptionField(L"");
	BlueprintBooleanProperty boolProperty4;
	boolProperty4.setNameField(L"InvulAfterHeal");
	boolProperty4.setValueField(true);
	boolProperty4.setDisplayNameField(L"Make Invulnerable after heal");
	boolProperty4.setDescriptionField(L"");

	stringName_ = L"HP";
	propertyList_.push_back(floatProperty1);
	propertyList_.push_back(floatProperty2);
	propertyList_.push_back(floatProperty3);
	propertyList_.push_back(boolProperty1);
	propertyList_.push_back(boolProperty2);
	propertyList_.push_back(boolProperty3);
	propertyList_.push_back(boolProperty4);
}

BlueprintPropertyContainer::~BlueprintPropertyContainer()
{
}

void BlueprintPropertyContainer::init()
{
}

void BlueprintPropertyContainer::setNameField(const std::wstring & text)
{
	stringName_ = text;
}
void BlueprintPropertyContainer::getNameField(std::wstring * text) const
{
	*text = stringName_;
}



BlueprintModule::BlueprintModule()
	: stringName_(L"TestText")
	, stringType_(L"TestText2")
	, boolFinalized_(true)
{
	BlueprintPropertyContainer propContainer;
	propertyContainerList_.push_back(propContainer);
}

BlueprintModule::~BlueprintModule()
{
}

void BlueprintModule::init()
{
}

void BlueprintModule::setNameField(const std::wstring & text)
{
	stringName_ = text;
}
void BlueprintModule::getNameField(std::wstring * text) const
{
	*text = stringName_;
}

void BlueprintModule::setTypeField(const std::wstring & text)
{
	stringType_ = text;
}
void BlueprintModule::getTypeField(std::wstring * text) const
{
	*text = stringType_;
}

void BlueprintModule::setFinalizedField(const bool & checked)
{
	boolFinalized_ = checked;
}
void BlueprintModule::getFinalizedField(bool * checked) const
{
	*checked = boolFinalized_;
}



BlueprintPage::BlueprintPage()
{
	BlueprintModule testModule;
	testModule.setNameField(L"m7_schwarzkopf_v1");
	testModule.setTypeField(L"Tank");
	testModule.setFinalizedField(true);
	moduleList_.push_back(testModule);
}

BlueprintPage::~BlueprintPage()
{
}

void BlueprintPage::init()
{
}