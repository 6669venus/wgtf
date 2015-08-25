#include "pch.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_variant/default_meta_type_manager.hpp"


int main( int argc, char* argv[] )
{
	MetaTypeImpl<Collection> collectionMetaType;
	DefaultMetaTypeManager metaTypeManager;
	metaTypeManager.registerType(&collectionMetaType);
	Variant::setMetaTypeManager( &metaTypeManager );

	int ret = BWUnitTest::runTest( "variant", argc, argv );

	return ret;
}

// main.cpp
