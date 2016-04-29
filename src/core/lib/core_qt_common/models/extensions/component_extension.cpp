#include "component_extension.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include <QQmlComponent>
#include <QModelIndex>

ITEMROLE( component )

ComponentExtension::ComponentExtension()
	: qtFramework_( Context::queryInterface< IQtFramework >() )
{
}

ComponentExtension::~ComponentExtension()
{
}

QHash< int, QByteArray > ComponentExtension::roleNames() const
{
	QHash< int, QByteArray > roleNames;
	registerRole( ItemRole::componentName, roleNames );
	return roleNames;
}

QVariant ComponentExtension::data( const QModelIndex &index, int role ) const
{
	size_t roleId;
	if (!decodeRole( role, roleId ))
	{
		return QVariant( QVariant::Invalid );
	}

	if (roleId != ItemRole::componentId)
	{
		return QVariant( QVariant::Invalid );
	}

	std::function< Variant ( size_t ) > dataPredicate = [&] ( size_t role ) {
		return index.model()->data( index, static_cast< int >( role ) );
	};
	auto component = qtFramework_->findComponent( dataPredicate );
	if (component == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	auto qmlComponent = qtFramework_->toQmlComponent( *component );
	if (qmlComponent == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	return QVariant::fromValue< QObject * >( qmlComponent );
}

bool ComponentExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	return false;
}