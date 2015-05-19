CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( bw_adapters )

INCLUDE( BWStandardProject )
INCLUDE( BWStandardLibrary )

SET( ALL_SRCS
	appmgr/app.hpp
	common/cooperative_moo.hpp
	common/string_utils.hpp
	cstdmf/bw_string.hpp
	cstdmf/bw_set.hpp
	cstdmf/bw_vector.hpp
	cstdmf/bw_map.hpp
	cstdmf/string_utils.hpp
	cstdmf/init_singleton.hpp
	cstdmf/singleton.hpp
	cstdmf/singleton_manager.hpp
	cstdmf/singleton_manager.cpp
	cstdmf/watcher.hpp
	cstdmf/stack_tracker.hpp
	input/input.hpp
	math/vector2.hpp
	math/vector3.hpp
	math/vector4.hpp
	moo/render_context.hpp
	pyscript/script.hpp
	resmgr/datasection.hpp
	resmgr/bwresource.hpp
	resmgr/bwresource.cpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

SET( GUIMANAGER
	guimanager/i_menu_helper.hpp
	guimanager/gui_forward.hpp
	guimanager/gui_item.hpp
	guimanager/gui_manager.hpp
	guimanager/gui_functor.hpp
	guimanager/gui_functor_cpp.hpp
	guimanager/gui_functor_option.hpp
	guimanager/gui_functor_watcher.hpp
	guimanager/gui_functor_python.hpp
	guimanager/gui_functor_datasection.hpp
	guimanager/gui_bitmap.hpp
)
SOURCE_GROUP( "GUIManager" FILES ${GUIMANAGER} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${GUIMANAGER}
)
BW_ADD_LIBRARY( bw_adapters ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( bw_adapters "Adapters" )

