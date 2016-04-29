#ifndef REFLECTION_COMPONENT_PROVIDER_HPP
#define REFLECTION_COMPONENT_PROVIDER_HPP

#include "core_ui_framework/i_component_provider.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item_role.hpp"

ITEMROLE( valueType )

class ReflectionComponentProvider : public IComponentProvider
{
public:
	ReflectionComponentProvider( IDefinitionManager & defManager )
		: defManager_( defManager )
	{
	}

	const char * component( std::function< Variant ( size_t ) > & dataPredicate ) const override
	{
		auto data = dataPredicate( ItemRole::valueTypeId );

		ObjectHandle handle;
		if (!data.tryCast( handle ))
		{
			return nullptr;
		}

		auto typeId = handle.getBase< TypeId >();
		if (typeId == nullptr)
		{
			return nullptr;
		}

		if (!typeId->isPointer())
		{
			return nullptr;
		}

		auto typeDef = defManager_.getDefinition( typeId->removePointer().getName() );
		if (typeDef == nullptr)
		{
			return nullptr;
		}

		return "polystruct";
	}

	// DEPRECATED //
	const char * componentId( const TypeId & typeId,
		std::function< bool ( size_t ) > & predicate ) const
	{
		if (typeId.isPointer())
		{
			auto typeDef = defManager_.getDefinition( typeId.removePointer().getName() );
			if (typeDef != nullptr)
			{
				return "polystruct";
			}
		}

		return nullptr;
	}

public:
	IDefinitionManager & defManager_;
};

#endif//REFLECTION_COMPONENT_PROVIDER_HPP