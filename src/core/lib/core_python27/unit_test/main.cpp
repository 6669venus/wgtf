#include "pch.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_variant/default_meta_type_manager.hpp"

int main( int argc, char* argv[] )
{
	DefaultMetaTypeManager metaTypeManager;
	Variant::setMetaTypeManager( &metaTypeManager );

	int result = BWUnitTest::runTest( "python27 test", argc, argv );
	return result;
}
