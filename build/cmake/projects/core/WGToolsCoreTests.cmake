SET( CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_MODULE_PATH}
)

INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR}/core/lib )

IF( BW_UNIT_TESTS_ENABLED )
	LIST( APPEND BW_TOOLS_UNIT_TEST_LIBRARIES
		reflection_test_objects		core/lib/reflection_objects_test
		core_unit_test		core/lib/core_unit_test
		CppUnitLite2		core/third_party/CppUnitLite2
	)

	LIST( APPEND BW_TOOLS_UNIT_TEST_BINARIES
		gps_unit_test 						core/lib/core_generic_plugin_manager/unit_test
		variant_unit_test 					core/lib/core_variant/unit_test
		command_system_unit_test 			core/lib/core_command_system/unit_test
		serialization_unit_test 			core/lib/core_serialization/unit_test
		core_common_unit_test 			    core/lib/core_common/unit_test
		reflection_unit_test 				core/lib/core_reflection/unit_test

		)

	LIST( APPEND BW_TOOLS_UNIT_TEST_PLUGINS
		plg_plugin1_test 					core/lib/core_generic_plugin_manager/unit_test/plugin1_test
		plg_plugin2_test 					core/lib/core_generic_plugin_manager/unit_test/plugin2_test
		)

	MESSAGE( STATUS "Unit tests enabled for tools." )
	ENABLE_TESTING()
ENDIF()

LIST( APPEND BW_LIBRARY_PROJECTS
	# Unit test libs
	${BW_TOOLS_UNIT_TEST_LIBRARIES}
)

LIST( APPEND BW_BINARY_PROJECTS
	# Unit tests
	${BW_TOOLS_UNIT_TEST_BINARIES}
)

LIST( APPEND BW_PLUGIN_PROJECTS
	plg_test_interfaces			core/testing/interfaces
	plg_test_main				core/testing/plg_test_main
	plg_test_window				core/testing/plg_test_window
	plg_test_2					core/testing/plg_test_2
	plg_test_3					core/testing/plg_test_3
	plg_test_4					core/testing/plg_test_4
	plg_test_reflection			core/testing/plg_test_reflection
	plg_test_progress_manager	core/testing/plg_test_progress_manager
	plg_test_ui_main			core/testing/plg_test_ui_main
	plg_test_data_model			core/testing/plg_test_data_model
	plg_test_obj_handle			core/testing/plg_test_obj_handle
	plg_test_asset_browser		core/testing/plg_test_asset_browser

	# Unit test plugins
	${BW_TOOLS_UNIT_TEST_PLUGINS}
)

