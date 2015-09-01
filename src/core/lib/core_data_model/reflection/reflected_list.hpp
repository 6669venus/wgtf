#ifndef REFLECTED_LIST_HPP
#define REFLECTED_LIST_HPP

#include "core_data_model/variant_list.hpp"

class IDefinitionManager;
class PropertyAccessorListener;

class ReflectedList : public VariantList
{
public:
	ReflectedList( IDefinitionManager* defManager );
	virtual ~ReflectedList();

private:
	std::shared_ptr< PropertyAccessorListener > listener_;
	IDefinitionManager* defManager_;
};

#endif // REFLECTED_LIST_HPP
