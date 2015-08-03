#include "pch.hpp"

#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_variant/default_meta_type_manager.hpp"
#include "test_objects.hpp"


int main( int argc, char* argv[] )
{
	DefaultMetaTypeManager metaTypeManager;
	Variant::setMetaTypeManager( &metaTypeManager );

	std::vector<std::unique_ptr<MetaType>> metaTypes;
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
