#include "pch.hpp"
#include <stdlib.h>

#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_variant/default_meta_type_manager.hpp"
#include "test_objects.hpp"

int main( int argc, char* argv[] )
{
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	DefaultMetaTypeManager metaTypeManager;
	Variant::setMetaTypeManager( &metaTypeManager );

	std::vector<std::unique_ptr<MetaType>> metaTypes;
	metaTypes.emplace_back(new MetaTypeImpl<Vector3>);
	metaTypes.emplace_back(new MetaTypeImpl<Vector4>);
	metaTypes.emplace_back(new MetaTypeImpl<std::shared_ptr< BinaryBlock >>);
	metaTypes.emplace_back(new MetaTypeImpl<ObjectHandle>);

	for(const auto& m: metaTypes)
	{
		metaTypeManager.registerType(m.get());
	}

	int result = 0;
	result = BWUnitTest::runTest( "", argc, argv );

	return result;
}

// main.cpp
