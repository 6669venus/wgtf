#include "asset_browser_breadcrumbs_model.hpp"
#include "i_asset_object_item.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item_role.hpp"

//------------------------------------------------------------------------------
/*
AssetBreadcrumbItem::AssetBreadcrumbItem()
	: BaseBreadcrumbItem()
{
}

void AssetBreadcrumbItem::setAsset( const IAssetObjectItem * asset )
{
	asset_ = asset;
	fullPath_ = asset_->getFullPath();
	displayValue_ = asset_->getDisplayText(0); //gnelsontodo - do this instead: asset_->getData( 0, ValueRole::roleId_ );
}

void AssetBreadcrumbItem::addSubItem( const IAssetObjectItem & asset, IDefinitionManager & definitionManager )
{
	auto subItem = definitionManager.create< AssetBreadcrumbItem >();
	subItem->setAsset( &asset );
	subItems_.push_back( subItem );
}
*/

//------------------------------------------------------------------------------

struct AssetBrowserBreadcrumbsModel::Implementation
{
	Implementation( AssetBrowserBreadcrumbsModel& self, IDefinitionManager & definitionManager );

	AssetBrowserBreadcrumbsModel& self_;
	IDefinitionManager& definitionManager_;
	VariantList breadcrumbs_;

	BaseBreadcrumbItem * addBreadcrumb( const IAssetObjectItem * asset );
};

AssetBrowserBreadcrumbsModel::Implementation::Implementation( 
	AssetBrowserBreadcrumbsModel& self,
	IDefinitionManager & definitionManager )
: self_( self )
, definitionManager_( definitionManager )
{
}

BaseBreadcrumbItem * AssetBrowserBreadcrumbsModel::Implementation::addBreadcrumb( const IAssetObjectItem * asset )
{
	auto breadcrumb = definitionManager_.create< BaseBreadcrumbItem >();
	//gnelsontodo save for later? breadcrumb->setAsset( &asset );
	if (asset != nullptr)
	{
		breadcrumb->initialise( asset->getFullPath(), asset->getDisplayText( 0 ) ); //gnelsontodo use getData() later
	}

	//gnelsontodo - need to populate children here!
	self_.addSubItem( *breadcrumb.get(), asset );//gnelsontodo just a test!

	breadcrumbs_.push_back( breadcrumb );

	// Return a raw pointer to this new breadcrumb so sub items can be added
	return breadcrumb.get();
}

//------------------------------------------------------------------------------

AssetBrowserBreadcrumbsModel::AssetBrowserBreadcrumbsModel( IDefinitionManager & definitionManager )
: IBreadcrumbsModel()
, impl_( new Implementation( *this, definitionManager ) )
{
}

AssetBrowserBreadcrumbsModel::~AssetBrowserBreadcrumbsModel()
{
}

IListModel * AssetBrowserBreadcrumbsModel::getBreadcrumbs() const
{
	return &impl_->breadcrumbs_;
}

void AssetBrowserBreadcrumbsModel::clear()
{
	impl_->breadcrumbs_.clear();
}

BaseBreadcrumbItem * AssetBrowserBreadcrumbsModel::add( const IAssetObjectItem * asset )
{
	return impl_->addBreadcrumb( asset );
}

void AssetBrowserBreadcrumbsModel::addSubItem( BaseBreadcrumbItem & breadcrumb, const IAssetObjectItem * asset )
{
	VariantList& subItems = breadcrumb.getVariantSubItems();
	auto subBreadcrumb = impl_->definitionManager_.create< BaseBreadcrumbItem >();
	if (asset != nullptr)
	{
		subBreadcrumb->initialise( asset->getFullPath(), asset->getDisplayText( 0 ) ); //gnelsontodo use getData() later
	}

	subItems.push_back( subBreadcrumb );
}
