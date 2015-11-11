#ifndef ASSET_BROWSER_BREADCRUMBS_MODEL_HPP
#define ASSET_BROWSER_BREADCRUMBS_MODEL_HPP

#include "core_data_model/i_breadcrumbs_model.hpp"

class IDefinitionManager;
class IAssetObjectItem;

//------------------------------------------------------------------------------
// AssetBrowserBreadcrumbsModel
//
// An implementation of the IBreadcrumbsModel that is specific to the
// Asset Browser and manipulating IAssetObjectItems.
//------------------------------------------------------------------------------
/*gnelsontodo - save for later?
class AssetBreadcrumbItem : public BaseBreadcrumbItem
{
public:

	AssetBreadcrumbItem();
	virtual ~AssetBreadcrumbItem() {}

	virtual void setAsset( const IAssetObjectItem * asset );

	virtual void addSubItem( const IAssetObjectItem & asset, IDefinitionManager & definitionManager );

	virtual const IAssetObjectItem * getAsset() const { return asset_; }

private:

	const IAssetObjectItem * asset_;
};
*/

class AssetBrowserBreadcrumbsModel : public IBreadcrumbsModel
{
public:

	//
	// Lifecycle
	//

	AssetBrowserBreadcrumbsModel( IDefinitionManager & definitionManager );

	virtual ~AssetBrowserBreadcrumbsModel() override;
	
	//
	// IBreadcrumbsModel Implementation
	//

	virtual IListModel * getBreadcrumbs() const override;

	virtual void clear() override;

	virtual BaseBreadcrumbItem * add( const IAssetObjectItem * asset );

	virtual void addSubItem( BaseBreadcrumbItem & breadcrumb, const IAssetObjectItem * asset );

private:

	//
	// Private Implementation Principle
	//

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif //ASSET_BROWSER_BREADCRUMBS_MODEL_HPP