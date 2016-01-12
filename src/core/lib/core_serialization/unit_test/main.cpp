#include "pch.hpp"
#include <stdlib.h>
#include "core_variant/variant.hpp"
#include "core_variant/default_meta_type_manager.hpp"
#include "core_reflection/object_handle.hpp"

int main( int argc, char* argv[] )
{
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	DefaultMetaTypeManager metaTypeManager;
	std::vector<std::unique_ptr<MetaType>> metaTypes;
	metaTypes.emplace_back(new MetaTypeImpl<ObjectHandle>("object"));
	for(const auto& m: metaTypes)
	{
		metaTypeManager.registerType(m.get());
	}

	Variant::setMetaTypeManager( &metaTypeManager );

	int result = BWUnitTest::runTest(
			"serialization test", argc, argv );
	return result;
}

// main.cpp
