#include "qt_script_object.hpp"

#include "qt_common/helpers/qt_helpers.hpp"

#include "reflection/object_handle.hpp"
#include "reflection/interfaces/i_base_property.hpp"
#include "reflection/class_definition.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/interfaces/i_reflection_controller.hpp"

namespace
{
	PropertyAccessor bindProperty( ObjectHandle & object, int & propertyIndex )
	{
		assert( propertyIndex >= 0 );
		auto definition = object.getDefinition();
		if (definition == nullptr)
		{
			return PropertyAccessor();
		}

		auto properties = definition->allProperties();
		auto it = properties.begin();
		for (; propertyIndex > 0 && it != properties.end();)
		{
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
	int firstMethodIndex,
	QObject * parent )
	: QObject( parent )
	, controller_( contextManager )
	, metaObject_( metaObject )
	, object_( object )
	, firstMethodIndex_( firstMethodIndex )
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
	switch (c) 
	{
	case QMetaObject::InvokeMetaMethod:
		if (callMethod( id, argv ))
		{
			return id;
		}
		break;
	case QMetaObject::ReadProperty:
	case QMetaObject::WriteProperty:
		{
			// The property offset is in the base QObject
			if (id < metaObject_.propertyOffset())
			{
				return QObject::qt_metacall( c, id, argv );
			}
			else if (id == metaObject_.propertyOffset())
			{
				if (c == QMetaObject::ReadProperty)
				{
					auto value = reinterpret_cast< QObject ** >( argv[0] );
					*value = this;
				}

				return id;
			}

			// The property offset is in our QtScriptObject
			int propertyIndex = --id;

			auto property = bindProperty( object_, propertyIndex );
			if (property.isValid())
			{
				auto value = reinterpret_cast< QVariant * >( argv[0] );
				if (c == QMetaObject::ReadProperty)
				{
					*value = QtHelpers::toQVariant( 
						controller_->getValue( property ) );
				}
				else
				{
					auto oldValue = QtHelpers::toQVariant( 
						controller_->getValue( property ) );
					if (*value == oldValue)
					{
						return id;
					}

					RefObjectId objectId;
					Variant valueVariant = QtHelpers::toVariant( *value );
					controller_->setValue( property, valueVariant );
					emit propertyChanged( *value, id );
				}
				return id;
			}
			// not a property of this object. adjust the id and fall through
			id = propertyIndex;
		}
		break;
	}

	return QObject::qt_metacall( c, id, argv );
}

void QtScriptObject::propertyChanged( QVariant value, int id )
{
	void *argv[] = { nullptr, &value };
	QMetaObject::activate(this, metaObject(), id - 1, argv);
}


bool QtScriptObject::callMethod( int& id, void **argv )
{
	auto definition = object_.getDefinition();

	if (definition == nullptr)
	{
		return false;
	}

	int startIndex = firstMethodIndex_ + metaObject_.methodOffset();
	int count = metaObject_.methodCount();

	if (id < startIndex)
	{
		return false;
	}

	int max = std::min( startIndex + 2, count );

	if (id > max)
	{
		id -= max;
		return false;
	}

	id -= startIndex;
	QVariant* result = reinterpret_cast<QVariant*>( argv[0] );
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
				ObjectHandle handle( meta, &meta->getDefinition() );
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
				ObjectHandle handle( meta, &meta->getDefinition() );
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
	};

	return true;
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
