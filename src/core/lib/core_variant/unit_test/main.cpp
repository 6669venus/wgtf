#include "pch.hpp"
#include "variant/variant.hpp"
#include "variant/collection.hpp"
#include "variant/default_meta_type_manager.hpp"


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
