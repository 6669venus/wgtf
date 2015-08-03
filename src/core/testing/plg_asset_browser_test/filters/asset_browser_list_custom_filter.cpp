#include "filters/asset_browser_list_custom_filter.hpp"

#include "data_model/generic_list.hpp"
#include "data_model/i_item.hpp"
#include "data_model/i_item_role.hpp"
#include "data_model/asset_browser/folder_content_object_model.hpp"

#include <QString>
#include <QRegExp>


AssetBrowserListCustomFilter::AssetBrowserListCustomFilter()
{
}

AssetBrowserListCustomFilter::~AssetBrowserListCustomFilter()
{
}

bool AssetBrowserListCustomFilter::checkFilter( const IItem * item, const QString & filter )
{
	if (filter == "")
	{
		return true;
	}

	QRegExp reg( filter.toLower() );

	if (item->columnCount() >= 0)
	{
		ObjectHandle object;
		GenericListItem * listItem = static_cast< GenericListItem * >( const_cast< IItem * > ( item ) );
		Variant itemData = listItem->getData( 0, ValueRole::roleId_ );

		if (itemData.tryCast( object ))
		{
			FolderContentObjectModel * tmp = object.getBase< FolderContentObjectModel >();
			if (nullptr == tmp)
			{
				return false;
			}

			QString text = QString::fromStdString( tmp->getFileName() );

			if (reg.indexIn( text.toLower() ) != -1)
			{
				return true;
			}
		}
	}

	return false;
}

