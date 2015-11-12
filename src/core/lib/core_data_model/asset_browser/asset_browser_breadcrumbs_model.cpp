#include "asset_browser_breadcrumbs_model.hpp"
#include "i_asset_object_item.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/i_item_role.hpp"

//------------------------------------------------------------------------------

struct AssetBrowserBreadcrumbsModel::Implementation
{
	Implementation( AssetBrowserBreadcrumbsModel& self, IDefinitionManager & definitionManager );

	AssetBrowserBreadcrumbsModel& self_;
	IDefinitionManager& definitionManager_;
	VariantList breadcrumbs_;
	std::string rootValue_;
	std::string path_;

	BaseBreadcrumbItem * addBreadcrumb( const IAssetObjectItem * asset );
};

AssetBrowserBreadcrumbsModel::Implementation::Implementation( 
	AssetBrowserBreadcrumbsModel& self,
	IDefinitionManager & definitionManager )
: self_( self )
, definitionManager_( definitionManager )
, rootValue_( "res" )
, path_( "" )
{
}

BaseBreadcrumbItem * AssetBrowserBreadcrumbsModel::Implementation::addBreadcrumb( const IAssetObjectItem * asset )
{
	assert( asset != nullptr );

	auto breadcrumb = definitionManager_.create< BaseBreadcrumbItem >();

	const char * displayValue = asset->getDisplayText( 0 );
	if (strlen( displayValue ) == 0)
	{
		displayValue = rootValue_.c_str();
	}
	
	breadcrumb->initialise( asset->getFullPath(), displayValue );

	// Find the children of this asset, if any exist. Also add the asset itself as an option (Windows Explorer)
	self_.addSubItem( *breadcrumb, asset );

	size_t size = asset->size();
	for (size_t i = 0; i < size; ++i)
	{
		auto childAssetItem = dynamic_cast< IAssetObjectItem* >( (*asset)[i] );
		self_.addSubItem( *breadcrumb, childAssetItem );
	}

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

const char * AssetBrowserBreadcrumbsModel::getPath() const
{
	return impl_->path_.c_str();
}

void AssetBrowserBreadcrumbsModel::setPath( const char * path )
{
	impl_->path_ = path;
}

void AssetBrowserBreadcrumbsModel::clear()
{
	impl_->breadcrumbs_.clear();
}

size_t AssetBrowserBreadcrumbsModel::size() const
{
	return impl_->breadcrumbs_.size();
}

BaseBreadcrumbItem * AssetBrowserBreadcrumbsModel::add( const IAssetObjectItem * asset )
{
	return impl_->addBreadcrumb( asset );
}

void AssetBrowserBreadcrumbsModel::addSubItem( BaseBreadcrumbItem & parent, const IAssetObjectItem * asset )
{
	auto subBreadcrumb = impl_->definitionManager_.create< BaseBreadcrumbItem >();

	const char * displayValue = asset->getDisplayText( 0 );
	if (strlen( displayValue ) == 0)
	{
		displayValue = impl_->rootValue_.c_str();
	}
	
	subBreadcrumb->initialise( asset->getFullPath(), displayValue );
	parent.addSubItem( subBreadcrumb );
}
