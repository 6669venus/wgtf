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
	std::string path_;

	BaseBreadcrumbItem * addBreadcrumb( const IAssetObjectItem * asset );

	BaseBreadcrumbItem * getSubItem( const BaseBreadcrumbItem * parent, unsigned int index );
};

AssetBrowserBreadcrumbsModel::Implementation::Implementation( 
	AssetBrowserBreadcrumbsModel& self,
	IDefinitionManager & definitionManager )
: self_( self )
, definitionManager_( definitionManager )
, path_( "" )
{
}

BaseBreadcrumbItem * AssetBrowserBreadcrumbsModel::Implementation::addBreadcrumb( const IAssetObjectItem * asset )
{
	assert( asset != nullptr );

	auto breadcrumb = definitionManager_.create< BaseBreadcrumbItem >();
	breadcrumb->initialise( *asset, asset->getFullPath(), asset->getDisplayText( 0 ) );

	// Find the children of this asset, if any exist. Also add the asset itself as an option (Windows Explorer)
	self_.addSubItem( *breadcrumb, asset );

	size_t size = asset->size();
	for (size_t i = 0; i < size; ++i)
	{
		// Static cast used here, because we know that we will only ever get IAssetObjectItems from within an
		// IAssetObjectItem's children.
		auto childAssetItem = static_cast< IAssetObjectItem* >( (*asset)[i] );
		self_.addSubItem( *breadcrumb, childAssetItem );
	}

	breadcrumbs_.push_back( breadcrumb );

	// Return a raw pointer to this new breadcrumb so sub items can be added
	return breadcrumb.get();
}

BaseBreadcrumbItem * AssetBrowserBreadcrumbsModel::Implementation::getSubItem( const BaseBreadcrumbItem * parent, 
	unsigned int index )
{
	assert( parent != nullptr );

	const auto subItemVariant = parent->getSubItem( index );
	if (subItemVariant->typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (subItemVariant->tryCast( provider ))
		{
			return provider.getBase< BaseBreadcrumbItem >();
		}
	}

	return nullptr;
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

Variant AssetBrowserBreadcrumbsModel::getItemAtIndex( unsigned int index, unsigned int childIndex )
{
	if (index < static_cast< unsigned int >( size() ))
	{
		auto variant = impl_->breadcrumbs_[ index ];	
		if (variant.typeIs< ObjectHandle >())
		{
			ObjectHandle provider;
			if (variant.tryCast( provider ))
			{
				auto breadcrumb = provider.getBase< BaseBreadcrumbItem >();
				if (breadcrumb != nullptr && 
					childIndex < static_cast< unsigned int >( breadcrumb->getSubItems()->size() ))
				{
					if (childIndex == 0)
					{
						// Subitem index 0 is always itself
						return Variant( reinterpret_cast< uintptr_t >( breadcrumb->getItem() ) );
					}
					else
					{
						// Get the subitem at the specified childindex
						auto childcrumb = impl_->getSubItem( breadcrumb, childIndex );
						assert( childcrumb != nullptr );
						return Variant( reinterpret_cast< uintptr_t >( childcrumb->getItem() ) );
					}
				}
			}
		}
	}

	return Variant();
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
	subBreadcrumb->initialise( *asset, asset->getFullPath(), asset->getDisplayText( 0 ) );
	parent.addSubItem( subBreadcrumb );
}
