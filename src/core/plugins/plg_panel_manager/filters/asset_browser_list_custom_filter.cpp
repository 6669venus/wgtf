#include "filters/asset_browser_list_custom_filter.hpp"

#include "core_data_model/generic_list.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/asset_browser/file_object_model.hpp"

#include <QString>
#include <QRegExp>


AssetBrowserListCustomFilter::AssetBrowserListCustomFilter()
{
}

AssetBrowserListCustomFilter::~AssetBrowserListCustomFilter()
{
}

bool AssetBrowserListCustomFilter::checkFilter( const IItem * item, const std::vector<QString> & filters )
{
	if (filters.size() < 1)
	{
		return true;
	}

	bool checkFilterPassed = true;

	for (auto & filter : filters)
	{
		QRegExp reg( filter, Qt::CaseInsensitive );

		if (item->columnCount() >= 0)
		{
			ObjectHandle object;
			GenericListItem * listItem = static_cast< GenericListItem * >( const_cast< IItem * > ( item ) );
			Variant itemData = listItem->getData( 0, ValueRole::roleId_ );

			if (itemData.tryCast( object ))
			{
				auto tmp = object.getBase< FileObjectModel >();
				if (nullptr == tmp)
				{
					// Invalid object
					return false;
				}

				QString text = QString::fromStdString( tmp->getFileName() );

				checkFilterPassed &= (reg.indexIn( text ) != -1);
			}
		}
	}

	return checkFilterPassed;
}

