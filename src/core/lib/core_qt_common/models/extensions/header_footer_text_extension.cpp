#include "header_footer_text_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_qt_common/models/wg_tree_model.hpp"


HeaderFooterTextExtension::HeaderFooterTextExtension()
{
}


HeaderFooterTextExtension::~HeaderFooterTextExtension()
{
}


QHash<int, QByteArray> HeaderFooterTextExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( headerTextRole::role_, roleNames );
	registerRole( footerTextRole::role_, roleNames );
	return roleNames;
}


QVariant HeaderFooterTextExtension::headerData( int column, int role ) const
{
	size_t roleId;

	if (!decodeRole( role, roleId ))
	{
		return QVariant::Invalid;
	}

	if (roleId != headerTextRole::roleId_ && roleId != footerTextRole::roleId_)
	{
		return QVariant::Invalid;
	}

	// TODO: find a way to avoid these casts.
	// We could have a common ancestor to ITreeModel and IListModel called IDataModel.
	// We would need a common ancestor to WGTreeModel and WGListModel called WGDataModel.
	// WGDataModel could be a template class on either ITreeModel or IListModel, or it could be based on IDataModel*.
	WGListModel* listModel = dynamic_cast<WGListModel*>( model_ );

	if (listModel != nullptr)
	{
		return QtHelpers::toQVariant(
			listModel->getModel()->getData( column, roleId ) );
	}

	WGTreeModel* treeModel = dynamic_cast<WGTreeModel*>( model_ );

	if (treeModel != nullptr)
	{
		return QtHelpers::toQVariant(
			treeModel->getModel()->getData( column, roleId ) );
	}

	return QVariant::Invalid;
}
