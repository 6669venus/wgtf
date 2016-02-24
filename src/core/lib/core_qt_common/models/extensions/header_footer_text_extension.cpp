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
