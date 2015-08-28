#include "filters/asset_browser_list_custom_filter.hpp"

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
		auto listItem = const_cast< IItem * > ( item );
		Variant itemData = listItem->getData( 0, ValueRole::roleId_ );

		if (itemData.tryCast( object ))
		{
			auto tmp = object.getBase< FileObjectModel >();
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

