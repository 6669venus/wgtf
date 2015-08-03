#ifndef QT_HELPERS_HPP
#define QT_HELPERS_HPP

#include "variant/variant.hpp"

#include <QVariant>

class ObjectHandle;
class TypeId;
class QQuickItem;

namespace QtHelpers
{
	QVariant toQVariant( const Variant & variant );
	QVariant toQVariant( const ObjectHandle & object );
	Variant toVariant( const QVariant & qVariant );

	QVariant convertToQTTypeInstance( const TypeId & type, const void * data );
	QQuickItem * findChildByObjectName(
		QObject * parent, const char * controlName );
};

#endif //QT_HELPERS_HPP