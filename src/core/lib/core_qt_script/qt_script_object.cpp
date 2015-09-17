#include "qt_script_object.hpp"

#include "core_qt_common/helpers/qt_helpers.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include "core_logging/logging.hpp"

namespace
{
	PropertyAccessor bindProperty( ObjectHandle & object, int & propertyIndex, IDefinitionManager & definitionManager, bool method = false )
	{
		assert( propertyIndex >= 0 );
		auto definition = object.getDefinition( definitionManager );
		if (definition == nullptr)
		{
			return PropertyAccessor();
		}

		auto properties = definition->allProperties();
		auto it = properties.begin();
		while (it->isMethod() != method && it != properties.end())
		{
			++it;
		}
		for (; propertyIndex > 0 && it != properties.end();)
		{
			if (it->isMethod() != method)
			{
				continue;
			}

			if (--propertyIndex == 0)
			{
				break;
			}
			++it;
		}
		if (it == properties.end())
		{
			return PropertyAccessor();
		}

		return definition->bindProperty( it->getName(), object );
	}
}

QtScriptObject::QtScriptObject(
	IComponentContext& contextManager,
	const QMetaObject & metaObject,
	const ObjectHandle & object,
	QObject * parent )
	: QObject( parent )
	, definitionManager_( contextManager )
	, controller_( contextManager )
	, metaObject_( metaObject )
	, object_( object )
{
}

QtScriptObject::~QtScriptObject()
{
}

const QMetaObject * QtScriptObject::metaObject() const
{
	return &metaObject_;
}

int QtScriptObject::qt_metacall( QMetaObject::Call c, int id, void **argv )
{
	id = QObject::qt_metacall( c, id, argv );

	if (id < 0)
	{
		return id;
	}

	switch (c) 
	{
	case QMetaObject::InvokeMetaMethod:
		{
			callMethod( id, argv );
			int methodCount = metaObject_.methodCount() - metaObject_.methodOffset();
			id -= methodCount;
			break;
		}
	case QMetaObject::ReadProperty:
	case QMetaObject::WriteProperty:
		{
			int propertyCount = metaObject_.propertyCount() - metaObject_.propertyOffset();

			if (id == 0)
			{
				if (c == QMetaObject::ReadProperty)
				{
					auto value = reinterpret_cast< QObject ** >( argv[0] );
					*value = this;
				}

				id -= propertyCount;
				return id;
			}

			// The property offset is in our QtScriptObject
			auto property = bindProperty( object_, id, *definitionManager_ );

			if (property.isValid())
			{
				auto value = reinterpret_cast< QVariant * >( argv[0] );

				if (c == QMetaObject::ReadProperty)
				{
					*value = QtHelpers::toQVariant( controller_->getValue( property ) );
				}
				else
				{
					auto oldValue = QtHelpers::toQVariant( controller_->getValue( property ) );

					if (*value != oldValue)
					{
						RefObjectId objectId;
						Variant valueVariant = QtHelpers::toVariant( *value );
						controller_->setValue( property, valueVariant );
						emit propertyChanged( *value, id );
					}
				}
			}

			// not a property of this object. adjust the id and fall through
			id -= propertyCount;
		}
		break;
	default:
		break;
	}

	return id;
}


void QtScriptObject::propertyChanged( QVariant value, int id )
{
	void *parameters[] = { nullptr, &value };
	callMethod( id - 1, parameters );
}


void QtScriptObject::callMethod( int id, void **argv )
{
	int methodCount = metaObject_.methodCount() - metaObject_.methodOffset();

	if (id >= methodCount)
	{
		return;
	}

	int propertyCount = metaObject_.propertyCount() - metaObject_.propertyOffset();
	int firstMethodSignalId = propertyCount - 1;
	int methodSignalCount = (methodCount - firstMethodSignalId - 1) / 2;
	int firstMethodId = firstMethodSignalId + methodSignalCount;

	if (id < firstMethodId)
	{
		metaObject_.activate( this, id + metaObject_.methodOffset(), argv );
		return;
	}

	id -= firstMethodId;
	QVariant* result = reinterpret_cast<QVariant*>( argv[0] );

	if (id < 3)
	{
		auto definition = object_.getDefinition( *definitionManager_ );

		if (definition == nullptr)
		{
			return;
		}

		QString* property = reinterpret_cast<QString*>( argv[1] );
		QString* metaType = (id == 0) ? nullptr : reinterpret_cast<QString*>( argv[2] );

		switch (id)
		{
		case 0:
			{
				const MetaBase* meta = getMetaObject( definition, *property );

				if (meta == nullptr)
				{
					*result = QVariant::Invalid;
				}
				else
				{
					auto handle = meta->getDefinition().getBaseProvider( meta );
					*result = QtHelpers::toQVariant( handle );
				}

				break;
			}
		case 1:
			{
				const MetaBase* meta = getMetaObject( definition, *property, *metaType );

				if (meta == nullptr)
				{
					*result = QVariant::Invalid;
				}
				else
				{
					auto handle = meta->getDefinition().getBaseProvider( meta );
					*result = QtHelpers::toQVariant( handle );
				}

				break;
			}
		case 2:
			{
				bool found = getMetaObject( definition, *property, *metaType ) != nullptr;
				*result = QtHelpers::toQVariant( Variant( found ) );
				break;
			}
		}
	}
	else
	{
		int methodId = id - 2;
		auto pa = bindProperty( object_, methodId, *definitionManager_, true );
		ReflectedMethodParameters parameters;

		for (size_t i = 0; i < pa.getProperty()->parameterCount(); ++i)
		{
			QVariant& qvariant = *reinterpret_cast<QVariant*>( argv[1 + i] );
			parameters.push_back( QtHelpers::toVariant( qvariant ) );
		}

		pa.invoke( parameters );
	}

	{// fire signal
		int signalId = id + firstMethodSignalId - 1;

		// the first two methods have the same name
		if (id == 0)
		{
			++ signalId;
		}

		void* parameters[] = {nullptr};
		callMethod( signalId, parameters );
	}

	return;
}


const MetaBase* QtScriptObject::getMetaObject(
	const IClassDefinition* definition,
	const QString& property ) const
{
	const MetaBase* meta = nullptr;
	
	if (property == "")
	{
		meta = definition->getMetaData();
	}
	else
	{
		auto properties = definition->allProperties();

		for (auto itr = properties.begin(); itr != properties.end(); ++itr)
		{
			if (property == itr->getName())
			{
				meta = itr->getMetaData();
				break;
			}
		}
	}

	return meta;
}


const MetaBase* QtScriptObject::getMetaObject(
	const IClassDefinition* definition,
	const QString& property,
	const QString& metaType ) const
{
	const MetaBase* metaObject = getMetaObject( definition, property );
	QString propertyName = "class Meta" + metaType + "Obj";

	while (metaObject != nullptr && propertyName != metaObject->getDefinitionName())
	{
		metaObject = metaObject->next();
	}

	return metaObject;
}


const ObjectHandle & QtScriptObject::object() const
{ 
	return object_; 
}
