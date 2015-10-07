SET( CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_MODULE_PATH}
)

INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR}/core/lib )

IF( BW_UNIT_TESTS_ENABLED )
	LIST( APPEND BW_TOOLS_UNIT_TEST_LIBRARIES
		reflection_test_objects		core/testing/reflection_objects_test
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
		data_model_unit_test				core/lib/core_data_model/unit_test

		)

	IF(MSVC)
		LIST(APPEND BW_TOOLS_UNIT_TEST_BINARIES
			test_plg_perforce 				core/testing/test_plg_perforce
		)
	ENDIF()

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
	interfaces_test			        core/testing/interfaces_test
	plg_main_test				    core/testing/plg_main_test
	plg_window_test				    core/testing/plg_window_test
	plg_2_test					    core/testing/plg_2_test
	plg_3_test					    core/testing/plg_3_test
	plg_4_test					    core/testing/plg_4_test
	plg_reflection_test			    core/testing/plg_reflection_test
	plg_progress_manager_test	    core/testing/plg_progress_manager_test
	plg_python27_test			    core/testing/plg_python27_test
	plg_ui_main_test			    core/testing/plg_ui_main_test
    plg_tree_expansion_status_test  core/testing/plg_tree_expansion_status_test
    plg_list_model_test  			core/testing/plg_list_model_test
	plg_tree_model_test  			core/testing/plg_tree_model_test
	plg_data_model_test			    core/testing/plg_data_model_test
	plg_obj_handle_test			    core/testing/plg_obj_handle_test
	plg_panel_manager_test		    core/testing/plg_panel_manager_test
	plg_test_active_filters		    core/testing/plg_test_active_filters

	# Unit test plugins
	${BW_TOOLS_UNIT_TEST_PLUGINS}
)

SET( NGT_PLUGIN_CONFIGURATION_FILES
	config/testing/*.txt
)
