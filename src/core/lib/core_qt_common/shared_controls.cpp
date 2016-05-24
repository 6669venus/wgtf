#include "shared_controls.hpp"

#include "controls/bw_filedialog_qi.hpp"
#include "controls/bw_copyable.hpp"
#include "controls/wg_context_menu.hpp"
#include "controls/wg_action.hpp"
#include "controls/wg_item_view.hpp"
#include "helpers/wg_filtered_list_model.hpp"
#include "helpers/wg_filtered_tree_model.hpp"
#include "helpers/wg_filter.hpp"
#include "helpers/wg_string_filter.hpp"
#include "helpers/wg_tokenized_string_filter.hpp"
#include "models/adapters/sequence_list_adapter.hpp"
#include "models/data_change_notifier.hpp"
#include "models/extensions/asset_item_extension.hpp"
#include "models/extensions/column_extension.hpp"
#include "models/extensions/column_extension_old.hpp"
#include "models/extensions/component_extension.hpp"
#include "models/extensions/component_extension_old.hpp"
#include "models/extensions/list_extension.hpp"
#include "models/extensions/selection_extension.hpp"
#include "models/extensions/thumbnail_extension.hpp"
#include "models/extensions/tree_extension.hpp"
#include "models/extensions/tree_extension_old.hpp"
#include "models/extensions/value_extension.hpp"
#include "models/extensions/image_extension.hpp"
#include "models/extensions/header_footer_text_extension.hpp"
#include "models/wg_list_model.hpp"
#include "models/wg_tree_model.hpp"
#include "models/wg_tree_list_adapter.hpp"
#include "helpers/selection_helper.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model.mpp"
#include "core_data_model/asset_browser/i_asset_browser_view_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_view_model.mpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model.mpp"
#include "core_data_model/asset_browser/asset_browser_breadcrumbs_model.hpp"
#include "core_data_model/i_active_filters_model.hpp"
#include "core_data_model/i_active_filters_model.mpp"
#include "core_data_model/i_breadcrumbs_model.hpp"
#include "core_data_model/i_breadcrumbs_model.mpp"
#include <QtQuick>

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"


//==============================================================================
/*static */void SharedControls::init()
{
	qmlRegisterType< DataChangeNotifier, 1 >( "BWControls", 1, 0, "BWDataChangeNotifier" );
	qmlRegisterType< SelectionHelper, 1 >( "WGControls", 1, 0, "SelectionHelper" );

	qmlRegisterType< BWFileDialogQI, 1 >( "BWControls", 1, 0, "BWFileDialogQI" );
	qmlRegisterType< BWCopyable, 1 >( "BWControls", 1, 0, "BWCopyable" );

	qmlRegisterType< WGListModel, 1 >( "WGControls", 1, 0, "WGListModel" );
	qmlRegisterType< WGTreeModel, 1 >( "WGControls", 1, 0, "WGTreeModel" );
	qmlRegisterType< WGFilteredTreeModel, 1 >( "WGControls", 1, 0, "WGFilteredTreeModel" );
	qmlRegisterType< WGFilteredListModel, 1 >( "WGControls", 1, 0, "WGFilteredListModel" );
	qmlRegisterType< WGContextMenu, 1 >( "WGControls", 1, 0, "WGContextMenu" );
	qmlRegisterType< WGAction, 1 >( "WGControls", 1, 0, "WGAction" );

	qmlRegisterType< SequenceListAdapter, 1 >("BWControls", 1, 0, "SequenceList" );
	qmlRegisterType< WGTreeListAdapter, 1 >( "WGControls", 1, 0, "WGTreeListAdapter" );

	qmlRegisterType< ColumnExtensionOld, 1 >( "WGControls", 1, 0, "ColumnExtension" );
	qmlRegisterType< ComponentExtensionOld, 1 >( "WGControls", 1, 0, "ComponentExtension" );
	qmlRegisterType< SelectionExtension, 1 >( "WGControls", 1, 0, "SelectionExtension" );
	qmlRegisterType< ThumbnailExtension, 1 >( "WGControls", 1, 0, "ThumbnailExtension" );
	qmlRegisterType< TreeExtensionOld, 1 >( "WGControls", 1, 0, "TreeExtension" );
	qmlRegisterType< ValueExtension, 1 >( "WGControls", 1, 0, "ValueExtension" );
	qmlRegisterType< AssetItemExtension, 1 >( "WGControls", 1, 0, "AssetItemExtension" );
	qmlRegisterType< HeaderFooterTextExtension, 1 >( "WGControls", 1, 0, "HeaderFooterTextExtension" );

	qmlRegisterType< ColumnExtension, 1 >( "WGControls", 2, 0, "ColumnExtension" );
	qmlRegisterType< ListExtension, 1 >( "WGControls", 2, 0, "ListExtension" );
	qmlRegisterType< TreeExtension, 1 >( "WGControls", 2, 0, "TreeExtension" );
	qmlRegisterType< ImageExtension, 1 >( "WGControls", 2, 0, "ImageExtension" );
	qmlRegisterType< ComponentExtension, 1 >( "WGControls", 2, 0, "ComponentExtension" );

	qmlRegisterType< WGItemView, 1 >( "WGControls", 2, 0, "WGItemView" );

	qmlRegisterType< WGFilter, 1 >( "WGControls", 1, 0, "WGFilter" );
	qmlRegisterType< WGStringFilter, 1 >( "WGControls", 1, 0, "WGStringFilter" );
	qmlRegisterType< WGTokenizedStringFilter, 1 >( "WGControls", 1, 0, "WGTokenizedStringFilter" );
}


//==============================================================================
/*static */void SharedControls::initDefs(
	IDefinitionManager & definitionManager )
{
	definitionManager.registerDefinition<TypeClassDefinition< IAssetBrowserModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< IAssetBrowserViewModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< IAssetBrowserEventModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< IActiveFiltersModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< ActiveFilterTerm >>();
	definitionManager.registerDefinition<TypeClassDefinition< SavedActiveFilter >>();
	definitionManager.registerDefinition<TypeClassDefinition< IBreadcrumbsModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< BaseBreadcrumbItem >>();
}
