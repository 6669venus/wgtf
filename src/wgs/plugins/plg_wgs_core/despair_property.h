#ifndef DESPAIR_PROPERTY_HPP
#define DESPAIR_PROPERTY_HPP

#include <memory>
#include "game_common.h"

class DespairProperty
	: public BaseProperty
{
public:

	DespairProperty(const char * name, const TypeId & type, const Despair::Reflection::ClassField & field);
	const MetaBase * getMetaDisplayName() const;

	bool set(const ObjectHandle & handle, const Variant & value) const override;
	Variant get(const ObjectHandle & handle) const;

private:

	const Despair::Reflection::ClassField & field_;
	std::wstring wideDisplayName_;
	MetaBase * metaDisplayName_;
};

#endif //DESPAIR_PROPERTY_HPP
