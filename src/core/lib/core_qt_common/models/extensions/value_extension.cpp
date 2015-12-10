#include "value_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"

ValueExtension::ValueExtension()
{
	
}

ValueExtension::~ValueExtension()
{
}

QHash< int, QByteArray > ValueExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( ValueRole::role_, roleNames );
	registerRole( ValueTypeRole::role_, roleNames );
	registerRole( EnumModelRole::role_, roleNames );
	registerRole( DefinitionRole::role_, roleNames );
	registerRole( DefinitionModelRole::role_, roleNames );
	registerRole( MinValueRole::role_, roleNames );
	registerRole( MaxValueRole::role_, roleNames );
	registerRole( StepSizeRole::role_, roleNames);
	registerRole( DecimalsRole::role_, roleNames);
	registerRole( IndexPathRole::role_, roleNames );
	registerRole( UrlIsAssetBrowserRole::role_, roleNames );
	registerRole( UrlDialogTitleRole::role_, roleNames );
	registerRole( UrlDialogDefaultFolderRole::role_, roleNames );
	registerRole( UrlDialogNameFiltersRole::role_, roleNames );
	registerRole( UrlDialogSelectedNameFilterRole::role_, roleNames );
	registerRole( UrlDialogModalityRole::role_, roleNames );

	return roleNames;
}

QVariant ValueExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	assert( index.isValid() );
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ValueRole::roleId_ ||
		roleId == ValueTypeRole::roleId_ ||
		roleId == IndexPathRole::roleId_ ||
		roleId == MinValueRole::roleId_ ||
		roleId == MaxValueRole::roleId_ ||
		roleId == StepSizeRole::roleId_ ||
		roleId == DecimalsRole::roleId_ ||
		roleId == EnumModelRole::roleId_ ||
		roleId == DefinitionRole::roleId_ ||
		roleId == DefinitionModelRole::roleId_ ||
		roleId == UrlIsAssetBrowserRole::roleId_ ||
		roleId == UrlDialogTitleRole::roleId_ ||
		roleId == UrlDialogDefaultFolderRole::roleId_ ||
		roleId == UrlDialogNameFiltersRole::roleId_ ||
		roleId == UrlDialogSelectedNameFilterRole::roleId_ ||
		roleId == UrlDialogModalityRole::roleId_ )
	{
		return QtHelpers::toQVariant( 
			item->getData( column, roleId ) );
	}
	return QVariant( QVariant::Invalid );
}

bool ValueExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return false;
	}

	assert( index.isValid() );
	auto item = reinterpret_cast< IItem * >( index.internalPointer() );
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ValueRole::roleId_ ||
		roleId == DefinitionRole::roleId_)	{
		auto oldValue = QtHelpers::toQVariant(
			item->getData( column, roleId ) );
		if (value == oldValue)
		{
			return true;
		}

		auto data = QtHelpers::toVariant( value );
		return item->setData( column, roleId, data );
	}

	return false;
}

void ValueExtension::onDataAboutToBeChanged( const QModelIndex &index, int role, const QVariant &value )
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return;
	}

	if (roleId == DefinitionRole::roleId_)
	{
		QList<QPersistentModelIndex> parents;
		parents.append( index );
		emit model_->layoutAboutToBeChanged( parents, QAbstractItemModel::VerticalSortHint );
	}
}

void ValueExtension::onDataChanged( const QModelIndex &index, int role, const QVariant &value )
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return;
	}

	if (roleId == ValueRole::roleId_ ||
		roleId == DefinitionRole::roleId_)
	{
		QVector<int> roles;
		roles.append( role );
		emit model_->dataChanged( index, index, roles );
	}

	if (roleId == DefinitionRole::roleId_)
	{
		QList<QPersistentModelIndex> parents;
		parents.append( index );
		emit model_->layoutChanged( parents, QAbstractItemModel::VerticalSortHint );
	}
}
