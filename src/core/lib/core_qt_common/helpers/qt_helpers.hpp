#ifndef QT_HELPERS_HPP
#define QT_HELPERS_HPP

#include <string>
#include "core_variant/variant.hpp"
#include <QVariant>

class QObject;
class ObjectHandle;
class TypeId;
class QQuickItem;
class QUrl;
class QQmlEngine;

namespace QtHelpers
{
	QVariant toQVariant( const Variant & variant );
	QVariant toQVariant( const ObjectHandle & object );
	Variant toVariant( const QVariant & qVariant );

	QVariant convertToQTTypeInstance( const TypeId & type, const void * data );
	QQuickItem * findChildByObjectName(
		QObject * parent, const char * controlName );

	QUrl resolveQmlPath( const QQmlEngine & qmlEngine, const char * relativePath );
};

#endif //QT_HELPERS_HPP