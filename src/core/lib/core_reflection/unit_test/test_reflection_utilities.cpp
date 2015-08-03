#include "pch.hpp"

#include "core_unit_test/unit_test.hpp"

#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_types.hpp"


class Base_PolyStruct : public ReflectedPolyStruct
{
	DECLARE_REFLECTED
};

BEGIN_EXPOSE(Base_PolyStruct, ReflectedPolyStruct, MetaNone())
END_EXPOSE()

class Derived_PolyStruct : public Base_PolyStruct
{
	DECLARE_REFLECTED
};

BEGIN_EXPOSE(Derived_PolyStruct, Base_PolyStruct, MetaNone())
END_EXPOSE()

class Base_Object : public ReflectedPolyStruct
{
	DECLARE_REFLECTED
};

BEGIN_EXPOSE(Base_Object, ReflectedPolyStruct, MetaNone())
END_EXPOSE()

class Derived_Object : public Base_Object
{
	DECLARE_REFLECTED
};

BEGIN_EXPOSE(Derived_Object, Base_Object, MetaNone())
END_EXPOSE()


class ReflectionUtilitiesFixture
{
public:
	ObjectManager objManager;
	DefinitionManager defManager;
	
	ObjectHandleT< ReflectedPolyStruct > polyStruct;
	ObjectHandleT< Base_PolyStruct > basePolyStruct;
	ObjectHandleT< Derived_PolyStruct > derivedPolyStruct;

	ObjectHandleT< ReflectedPolyStruct > object;
	ObjectHandleT<Base_Object> baseObject;
	ObjectHandleT<Derived_Object> derivedObject;

	ReflectionUtilitiesFixture() :
		defManager( objManager )
	{
		objManager.init( &defManager );

		Reflection::initReflectedTypes( defManager );

		IDefinitionManager& definitionManager = defManager;
		REGISTER_DEFINITION( Base_PolyStruct );
		REGISTER_DEFINITION( Derived_PolyStruct );
		REGISTER_DEFINITION( Base_Object );
		REGISTER_DEFINITION( Derived_Object );

		polyStruct  = definitionManager.create< ReflectedPolyStruct >();
		basePolyStruct = definitionManager.create< Base_PolyStruct >();
		derivedPolyStruct = definitionManager.create< Derived_PolyStruct >();
		object = definitionManager.create<ReflectedPolyStruct>();
		baseObject = definitionManager.create<Base_Object>();
		derivedObject = definitionManager.create<Derived_Object>();
	}

	~ReflectionUtilitiesFixture()
	{ 
	}
};


TEST_F( ReflectionUtilitiesFixture, get_definition )
{
	IDefinitionManager * defManager =
		polyStruct->getDefinition().getDefinitionManager();

	CHECK_EQUAL(defManager->getDefinition< ReflectedPolyStruct >(), &polyStruct->getDefinition());
	CHECK_EQUAL(defManager->getDefinition< Base_PolyStruct >(), &basePolyStruct->getDefinition());
	CHECK_EQUAL(defManager->getDefinition< Derived_PolyStruct >(), &derivedPolyStruct->getDefinition());

	CHECK_EQUAL(defManager->getDefinition< ReflectedPolyStruct >(), &object->getDefinition());
	CHECK_EQUAL(defManager->getDefinition< Base_Object >(), &baseObject->getDefinition());
	CHECK_EQUAL(defManager->getDefinition< Derived_Object >(), &derivedObject->getDefinition());
}


TEST_F( ReflectionUtilitiesFixture, dynamic_casting )
{
	auto tempPolyStruct = derivedPolyStruct;

	IDefinitionManager * defManager =
		tempPolyStruct->getDefinition().getDefinitionManager();

	Derived_PolyStruct * tempDerivedPolyStruct = ReflectionUtilities::dynamicCast<Derived_PolyStruct>(tempPolyStruct.get() );
	CHECK(tempDerivedPolyStruct != NULL);
	CHECK(defManager->getDefinition< Derived_PolyStruct >() == &tempDerivedPolyStruct->getDefinition());
	CHECK(derivedPolyStruct.get() == tempDerivedPolyStruct);


	ObjectHandleT< ReflectedPolyStruct > tempObject = derivedObject;

	Derived_Object * tempDerivedObject = ReflectionUtilities::dynamicCast<Derived_Object>(tempObject.get() );
	CHECK(tempDerivedObject != NULL);
	CHECK(defManager->getDefinition< Derived_Object >() == &tempDerivedObject->getDefinition());
	CHECK(derivedObject == tempDerivedObject);
}

