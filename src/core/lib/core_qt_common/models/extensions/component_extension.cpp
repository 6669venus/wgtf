#include "component_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_variant/type_id.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include <QQmlComponent>
#include <QModelIndex>

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
	registerRole( ComponentRole::role_, roleNames );
	return roleNames;
}

QVariant ComponentExtension::data( const QModelIndex &index, int role ) const
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

	if (roleId == ComponentRole::roleId_)
	{
		auto data = item->getData( column, ValueTypeRole::roleId_ );

		std::string typeName;
		data.tryCast( typeName );
		if (typeName.empty())
		{
			return QVariant( QVariant::Invalid );
		}

		auto typeId = TypeId( typeName.c_str() );
		std::function< bool ( size_t ) > predicate = [&] ( size_t role ) {
			return item->getData( column, role ) == true;
		};
		auto component = qtFramework_->findComponent( typeId, predicate );
		if (component != nullptr)
		{
			auto qmlComponent = qtFramework_->toQmlComponent( *component );
			if (qmlComponent != nullptr)
			{
				return QVariant::fromValue< QObject * >( qmlComponent );
			}
		}
	}
	
	return QVariant( QVariant::Invalid );
}

bool ComponentExtension::setData( const QModelIndex &index, const QVariant &value, int role )
{
	return false;
}