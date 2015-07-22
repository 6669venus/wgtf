#ifndef BLUEPRINT_MODEL_HPP
#define BLUEPRINT_MODEL_HPP

#include "reflection/reflected_object.hpp"
#include "reflection/generic/generic_object.hpp"

#include "math/vector3.hpp"
#include "math/vector4.hpp"

#include <map>
#include <vector>
#include <memory>

class BlueprintBaseProperty : public ReflectedPolyStruct
{
	DECLARE_REFLECTED
public:
	BlueprintBaseProperty();
	~BlueprintBaseProperty();
	void init();

	void setNameField(const std::wstring & text);
	void getNameField(std::wstring * text) const;

	void setDisplayNameField(const std::wstring & text);
	void getDisplayNameField(std::wstring * text) const;

	void setDescriptionField(const std::wstring & text);
	void getDescriptionField(std::wstring * text) const;

private:
	std::wstring stringName_;
	std::wstring stringDisplayName_;
	std::wstring stringDescription_;
};

class BlueprintFloatProperty : public BlueprintBaseProperty
{
	DECLARE_REFLECTED
public:
	BlueprintFloatProperty();
	~BlueprintFloatProperty();
	void init();

	void setValueField(const float & num);
	void getValueField(float * num) const;

private:
	float floatValue_;
};

class BlueprintBooleanProperty : public BlueprintBaseProperty
{
	DECLARE_REFLECTED
public:
	BlueprintBooleanProperty();
	~BlueprintBooleanProperty();
	void init();

	void setValueField(const bool & checked);
	void getValueField(bool * checked) const;

private:
	bool boolValue_;
};

class BlueprintStringProperty : public BlueprintBaseProperty
{
	DECLARE_REFLECTED
public:
	BlueprintStringProperty();
	~BlueprintStringProperty();
	void init();

	void setValueField(const std::wstring & text);
	void getValueField(std::wstring * text) const;

private:
	std::wstring stringValue_;
};

class BlueprintPropertyContainer : public ReflectedPolyStruct
{
	DECLARE_REFLECTED
public:
	BlueprintPropertyContainer();
	~BlueprintPropertyContainer();
	void init();

	void setNameField(const std::wstring & text);
	void getNameField(std::wstring * text) const;

private:
	std::wstring stringName_;
	std::vector< BlueprintBaseProperty > propertyList_;

};

class BlueprintModule : public ReflectedPolyStruct
{
	DECLARE_REFLECTED
public:
	BlueprintModule();
	~BlueprintModule();
	void init();

	void setNameField(const std::wstring & text);
	void getNameField(std::wstring * text) const;

	void setTypeField(const std::wstring & text);
	void getTypeField(std::wstring * text) const;

	void setFinalizedField(const bool & checked);
	void getFinalizedField(bool * checked) const;

private:
	std::wstring stringName_;
	std::wstring stringType_;
	bool boolFinalized_;

	std::vector< BlueprintPropertyContainer > propertyContainerList_;
};

class BlueprintPage : public ReflectedPolyStruct
{
	DECLARE_REFLECTED
public:
	BlueprintPage();
	~BlueprintPage();
	void init();

private:
	std::vector< BlueprintModule > moduleList_;
};

#endif // BLUEPRINT_MODEL_HPP
