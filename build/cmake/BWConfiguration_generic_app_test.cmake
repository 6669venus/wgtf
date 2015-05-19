SET( BW_SINGLETON_MANAGER_SUPPORT ON )
SET( NGT_SUPPORT ON )

ADD_DEFINITIONS( -DEDITOR_ENABLED )
ADD_DEFINITIONS( -DALLOW_STACK_CONTAINER )

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/lib )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw )
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/bw/lib )

SET( TARGET_PLUGIN_APP_FOLDER_NAME generic_app_test )

IF( BW_UNIT_TESTS_ENABLED )
	SET( BW_TOOLS_UNIT_TEST_LIBRARIES
		unit_test_lib		core/lib/unit_test_lib
		CppUnitLite2		core/third_party/CppUnitLite2
	)

	SET( BW_TOOLS_UNIT_TEST_BINARIES
		generic_plugin_system_unit_test 	core/lib/generic_plugin_system/unit_test
		variant_unit_test 				core/lib/variant/unit_test
		command_system_unit_test 			core/lib/command_system/unit_test
		
		editor_shared_unit_test 			bw/lib/editor_shared/unit_test
		reflection_unit_test 				bw/testing/reflection_unit_test
		)

	SET( BW_TOOLS_UNIT_TEST_PLUGINS
		generic_plugin_system_unit_test_plugin1 	core/lib/generic_plugin_system/test_plugin1
		generic_plugin_system_unit_test_plugin2 	core/lib/generic_plugin_system/test_plugin2
		)

	MESSAGE( STATUS "Unit tests enabled for tools." )
	ENABLE_TESTING()
ENDIF()

SET( BW_LIBRARY_PROJECTS	
	# BigWorld Adapters
	bw_adapters 		bw/adapters

	#WG Systems
	wg_types			core/lib/wg_types 
	wg_memory			core/lib/wg_memory

	#NGT Systems
	variant				core/lib/variant
	dependency_system	core/lib/dependency_system
	command_system		core/lib/command_system
	control_system		core/lib/control_system
	reflection			core/lib/reflection
	reflection_utils	core/lib/reflection_utils
	serialization		core/lib/serialization
	string_utils		core/lib/string_utils

	#Tools Common
	logging				core/lib/logging
	generic_plugin_system	core/lib/generic_plugin_system
	qt_common			core/lib/qt_common
	qt_script			core/lib/qt_script
	data_model			core/lib/data_model
	ui_framework		core/lib/ui_framework
	
	editor_shared		bw/lib/editor_shared
	bw_qt_common		bw/lib/bw_qt_common

	reflection_test_objects	bw/testing/reflection_test_objects

	# Unit test libs
	${BW_TOOLS_UNIT_TEST_LIBRARIES}
)

SET( BW_BINARY_PROJECTS	
	# Apps
	generic_app			core/app/generic_app

	# Unit tests
	${BW_TOOLS_UNIT_TEST_BINARIES}
)

SET( BW_PLUGIN_PROJECTS
	# Plugins
	#plg_automation				core/plugins/plg_automation
	plg_reflection				core/plugins/plg_reflection
	plg_command_system			core/plugins/plg_command_system
	plg_type_system				core/plugins/plg_type_system
	plg_editor_interaction		core/plugins/plg_editor_interaction
	plg_control_system			core/plugins/plg_control_system
	plg_history_ui				core/plugins/plg_history_ui
	plg_macros_ui				core/plugins/plg_macros_ui
	plg_qt						core/plugins/plg_qt
	plg_serialization			core/plugins/plg_serialization
	
	plg_bw_core					bw/plugins/plg_bw_core
	
	plg_test_interfaces			core/testing/interfaces
	plg_test_main				core/testing/plg_test_main
	plg_test_window				core/testing/plg_test_window
	plg_test_2					core/testing/plg_test_2
	plg_test_3					core/testing/plg_test_3
	plg_test_4					core/testing/plg_test_4
	plg_test_reflection			core/testing/plg_test_reflection
	
	plg_test_ui_main			bw/testing/plg_test_ui_main

	# Unit test plugins
	${BW_TOOLS_UNIT_TEST_PLUGINS}
)

