#ifndef DESPAIR_CLASS_H
#define DESPAIR_CLASS_H

#include "game_common.h"
#include "reflection/function_property.hpp"
#include "reflection/interfaces/i_class_definition_details.hpp"
#include "reflection/interfaces/i_class_definition_modifier.hpp"
#include "reflection/utilities/definition_helpers.hpp"
#include "reflection/metadata/meta_impl.hpp"
#include "reflection/metadata/meta_utilities.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "string_utils/string_utils.hpp"
#include <memory>

class DespairClassDefinition
	: public IClassDefinitionDetails
{
public:

	//--------------------------------------------------------------------------
	explicit DespairClassDefinition(const Despair::Reflection::ClassInfo * classInfo);
	virtual ~DespairClassDefinition();

	//--------------------------------------------------------------------------
	void init(IClassDefinitionModifier & modifier) override;

	//--------------------------------------------------------------------------
	bool isAbstract() const override;

	//--------------------------------------------------------------------------
	bool isGeneric() const override;

	//--------------------------------------------------------------------------
	const char * getName() const override;

	//--------------------------------------------------------------------------
	const char * getParentName() const override;

	//--------------------------------------------------------------------------
	const char * getParentName(int index) const;

	//--------------------------------------------------------------------------
	const MetaBase * getMetaData() const override;

	//--------------------------------------------------------------------------
	ObjectHandle createBaseProvider(const ReflectedPolyStruct & polyStruct) const override;

	//--------------------------------------------------------------------------
	ObjectHandle createBaseProvider(const IClassDefinition & definition, const void * pThis) const override;

		//--------------------------------------------------------------------------
	ObjectHandle create(const IClassDefinition & definition) const override;
	
	//--------------------------------------------------------------------------
	//TODO - this will be removed when ::create() is ready
	/*ReflectedPolyStruct * createPolyStruct() const
	{
	return
	createPolyStructHelper< Type >( static_cast< Type * >( nullptr ) );
	}*/

	//--------------------------------------------------------------------------
	CastHelperCache * getCastHelperCache() const override;

	//--------------------------------------------------------------------------
	const char* convertFieldTypeName(const Despair::Reflection::ClassField & field) const;

	//--------------------------------------------------------------------------
	template< bool isPolyStruct >
	struct BaseProviderHelper
	{
		static ObjectHandle getBaseProvider(
			const void * pThis, const IClassDefinition * definition)
		{
			return ObjectHandle(
				*static_cast< const Type * >(pThis), definition);
		}
	};

	//--------------------------------------------------------------------------
	/*template<>
	struct BaseProviderHelper< false >
	{
	static ObjectHandle getBaseProvider(
	const void * pThis, const IClassDefinition * definition)
	{
	return ObjectHandle::makeStorageBackedProvider(
	*static_cast< const Type * >(pThis), definition);
	}
	};*/

public:

	// NGT constructs
	mutable CastHelperCache castHelperCache_;
	IClassDefinitionModifier * modifier_;
	std::wstring wideDisplayName_;
	MetaBase* metaDisplayName_;

	// Despair-specific construct that contains property (fields and methods)
	// information on a particular class.
	const Despair::Reflection::ClassInfo * classInfo_;
};

#endif //DESPAIR_CLASS_H
