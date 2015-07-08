#include "pch.hpp"
#include "unit_test_lib/unit_test.hpp"
#include <chrono>
#include <thread>

#include "reflection/definition_manager.hpp"
#include "reflection/object_handle.hpp"
#include "reflection/object_manager.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/reflection_macros.hpp"
#include "reflection/function_property.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/reflected_types.hpp"
#include "reflection/variant_handler.hpp"

#include "reflection_utils/command_system_property_setter.hpp"
#include "reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "reflection_utils/commands/metadata/set_reflectedproperty_command.mpp"
#include "reflection_utils/serializer/reflection_serializer.hpp"

#include "variant/default_meta_type_manager.hpp"

#include "data_model/reflection/reflected_list.hpp"
#include "data_model/i_item_role.hpp"

#include "command_system/command_system.hpp"
#include "command_system/command_manager.hpp"

#include "serialization/serializer/serialization_manager.hpp"


class TestObjectHandleFixture
{
public:
	ObjectManager objManager;
	DefinitionManager defManager;
	DefaultMetaTypeManager metaTypeManager;
	CommandManager commandManager;
	CommandSystemReflectionPropertySetter reflectionPropertySetter;
	SetReflectedPropertyCommand setReflectedPropertyCmd;
	SerializationManager serializationManager;
	std::unique_ptr< ReflectionSerializer > reflectionSerializer;
	std::unique_ptr<ReflectionStorageLookupHandler> variantStorageLookupHandler;
	std::unique_ptr< MetaTypeImpl< ObjectHandle > > baseProviderMetaType;

	TestObjectHandleFixture()
		: defManager( objManager )
		, commandManager( defManager, std::this_thread::get_id() )
	{
		objManager.init( &defManager );

		Reflection::initReflectedTypes( defManager );
		CommandSystem::initReflectedTypes( defManager );
		IDefinitionManager & definitionManager = defManager;
		REGISTER_DEFINITION( ReflectedPropertyCommandArgument );

		commandManager.init();
		commandManager.registerCommand( &setReflectedPropertyCmd );
		reflectionPropertySetter.init( commandManager );
		//commandManager.registerCommandStatusListener( this );

		Variant::setMetaTypeManager( &metaTypeManager );

		baseProviderMetaType.reset( new MetaTypeImpl<ObjectHandle>() );
		metaTypeManager.registerType( baseProviderMetaType.get() );

		variantStorageLookupHandler.reset( new ReflectionStorageLookupHandler(
			&defManager, baseProviderMetaType.get() ) );

		reflectionSerializer.reset( 
			new ReflectionSerializer( serializationManager, metaTypeManager, objManager ) );

		objManager.setSerializationManager( &serializationManager );
		for(auto type : reflectionSerializer->getSupportedType())
		{
			serializationManager.registerSerializer( type.getName(), reflectionSerializer.get() );
		}

		metaTypeManager.registerDynamicStorageHandler( *variantStorageLookupHandler );
	}
};

class Test1
{
	DECLARE_REFLECTED
public:
	Test1() : value_(0) {}
	Test1(int v) : value_(v) {}

	int value_;
};

BEGIN_EXPOSE( Test1, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
END_EXPOSE()

class Test2
{
	DECLARE_REFLECTED
public:
	Test2() : value_(0) {}
	Test2(int v) : value_(v), test1_(v) {}

	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE( Test2, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test1", test1_, MetaNone() )
END_EXPOSE()

class Test3
{
	DECLARE_REFLECTED
public:
	Test3() : value_(0) {}
	Test3(int v) : value_(v), test2_(v), vector_(v, 1) {}

	int value_;
	Test2 test2_;
	std::vector<Test1> vector_;
};

BEGIN_EXPOSE( Test3, MetaNone() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test2", test2_, MetaNone() )
	EXPOSE( "TestVec", vector_, MetaNone() )
END_EXPOSE()

TEST_F(TestObjectHandleFixture, unmanaged_object)
{
	IDefinitionManager& definitionManager = defManager;
	IClassDefinition* def1 = REGISTER_DEFINITION( Test1 );
	IClassDefinition* def2 = REGISTER_DEFINITION( Test2 );
	IClassDefinition* def3 = REGISTER_DEFINITION( Test3 );

	std::unique_ptr<Test3> test = std::unique_ptr<Test3>( new Test3(3) );
	ObjectHandle handle(test.get(), def3);

	CHECK_EQUAL(objManager.getObject(test.get()).isValid(), false);
	CHECK_EQUAL(objManager.getUnmanagedObject(test.get()).isValid(), false);

	reflectionPropertySetter.setDataValue( def3->bindProperty("Value", handle), Variant(5) );

	CHECK_EQUAL(objManager.getObject(test.get()).isValid(), false);
	CHECK_EQUAL(objManager.getUnmanagedObject(test.get()).getBase<Test3>(), test.get());

	reflectionPropertySetter.setDataValue( def3->bindProperty("Test2.Value", handle), Variant(7) );
	reflectionPropertySetter.setDataValue( def3->bindProperty("Test2.Test1.Value", handle), Variant(11) );
	reflectionPropertySetter.setDataValue( def3->bindProperty("TestVec[1].Value", handle), Variant(13) );

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK_EQUAL(5, test->value_);
	CHECK_EQUAL(7, test->test2_.value_);
	CHECK_EQUAL(11, test->test2_.test1_.value_);
	CHECK_EQUAL(13, test->vector_[1].value_);
}

class Test1Stack
{
	DECLARE_REFLECTED
public:
	Test1Stack() : value_(0) {}
	Test1Stack(int v) : value_(v) {}

private:
	int value_;
};

BEGIN_EXPOSE( Test1Stack, MetaOnStack() )
	EXPOSE( "Value", value_, MetaNone() )
END_EXPOSE()

class Test2Stack
{
	DECLARE_REFLECTED
public:
	Test2Stack() : value_(0) {}
	Test2Stack(int v) : value_(v), test1_(v) {}

private:
	int value_;
	Test1 test1_;
};

BEGIN_EXPOSE( Test2Stack, MetaOnStack() )
	EXPOSE( "Value", value_, MetaNone() )
	EXPOSE( "Test1", test1_, MetaNone() )
END_EXPOSE()

class GListTest
{
public:
	GListTest( IDefinitionManager* defManager ) : gl_(defManager) {}
	GListTest( const GListTest& ) : gl_(nullptr) { assert(false); }

	template <typename T>
	void addItem( T& t ) { gl_.emplace_back( t ); }

	ObjectHandle getList() const { return ObjectHandle( &gl_ ); }

	PropertyAccessor bindProperty( size_t index, IClassDefinition* def, const char* name )
	{
		return def->bindProperty( name, gl_[index].value<const Variant&>().cast<ObjectHandle>() );
	}

private:
	ReflectedList gl_;
};

TEST_F(TestObjectHandleFixture, on_stack_object)
{
	IDefinitionManager& definitionManager = defManager;
	REGISTER_DEFINITION( Test1 );
	IClassDefinition* def1 = REGISTER_DEFINITION( Test1Stack );
	IClassDefinition* def2 = REGISTER_DEFINITION( Test2Stack );

	std::unique_ptr<GListTest> glist = std::unique_ptr<GListTest>( new GListTest(&definitionManager) );
	glist->addItem( Test1Stack( 5 ) );
	glist->addItem( Test2Stack( 58 ) );
	glist->addItem( Test1Stack( 7 ) );

	reflectionPropertySetter.setDataValue(
		glist->bindProperty(0u, def1, "Value"), Variant(13) );
	reflectionPropertySetter.setDataValue(
		glist->bindProperty(1u, def2, "Test1.Value"), Variant(17) );
	reflectionPropertySetter.setDataValue(
		glist->bindProperty(2u, def1, "Value"), Variant(19) );

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK_EQUAL(glist->bindProperty(0u, def1, "Value").getValue(), 13);
	CHECK_EQUAL(glist->bindProperty(1u, def2, "Test1.Value").getValue(), 17);
	CHECK_EQUAL(glist->bindProperty(2u, def1, "Value").getValue(), 19);
}
