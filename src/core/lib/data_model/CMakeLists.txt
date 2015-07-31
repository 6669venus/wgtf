CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )
PROJECT( data_model )

SET( MODEL_SRCS
	collection_list_item.cpp
	collection_list_item.hpp
	collection_list_model.cpp
	collection_list_model.hpp
	collection_model.cpp
	collection_model.hpp
	filtered_list_model.cpp
	filtered_list_model.hpp
	filtered_tree_model.cpp
	filtered_tree_model.hpp
	generic_list.cpp
	generic_list.hpp
	generic_tree_model.cpp
	generic_tree_model.hpp
	generic_tree_item.cpp
	generic_tree_item.hpp
	i_list_model.hpp
	i_tree_model.cpp
	i_tree_model.hpp
	i_item.hpp
	i_item_role.cpp
	i_item_role.hpp
	value_change_notifier.hpp
)
SOURCE_GROUP( "DataModel" FILES ${SHARED_MODEL_SRCS} )

SET( MODEL_REFLECTION_SRCS
	reflection/class_definition_model.cpp
	reflection/class_definition_model.hpp
	reflection/reflected_enum_model.cpp
	reflection/reflected_enum_model.hpp
	reflection/reflected_group_item.cpp
	reflection/reflected_group_item.hpp
	reflection/reflected_item.cpp
	reflection/reflected_item.hpp
	reflection/reflected_object_item.cpp
	reflection/reflected_object_item.hpp
	reflection/reflected_property_item.cpp
	reflection/reflected_property_item.hpp
	reflection/reflected_tree_model.cpp
	reflection/reflected_tree_model.hpp
	reflection/reflected_list.cpp
	reflection/reflected_list.hpp
)
SOURCE_GROUP( "DataModel\\Reflection"
	FILES ${MODEL_REFLECTION_SRCS} )

SET( MODEL_ASSET_BROWSER_SRCS
	asset_browser/i_asset_browser_model.hpp
	asset_browser/folder_tree_model.cpp
	asset_browser/folder_tree_model.hpp
	asset_browser/folder_tree_item.cpp
	asset_browser/folder_tree_item.hpp
	asset_browser/folder_content_object_model.cpp
	asset_browser/folder_content_object_model.hpp
	asset_browser/i_asset_browser_model.mpp
	asset_browser/folder_content_object_model.mpp
)
SOURCE_GROUP( "DataModel\\Asset Browser"
	FILES ${MODEL_ASSET_BROWSER_SRCS} )

SET( ALL_SRCS
	${MODEL_SRCS}
	${MODEL_ASSET_BROWSER_SRCS}
	${MODEL_REFLECTION_SRCS}
)

BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)

BW_ADD_LIBRARY( data_model  ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( data_model INTERFACE
	reflection
)

BW_PROJECT_CATEGORY( data_model "NGT Tools Libs" )

