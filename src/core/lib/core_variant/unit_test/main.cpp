#include "pch.hpp"
#include <stdlib.h>
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_variant/default_meta_type_manager.hpp"


int main( int argc, char* argv[] )
{
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	MetaTypeImpl<Collection> collectionMetaType;
	DefaultMetaTypeManager metaTypeManager;
	metaTypeManager.registerType(&collectionMetaType);
	Variant::setMetaTypeManager( &metaTypeManager );

	int ret = BWUnitTest::runTest( "variant", argc, argv );

	return ret;
}

// main.cpp
