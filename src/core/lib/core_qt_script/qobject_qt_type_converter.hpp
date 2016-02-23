#ifndef QOBJECT_QT_TYPE_CONVERTER_HPP
#define QOBJECT_QT_TYPE_CONVERTER_HPP

#include "core_qt_common/i_qt_type_converter.hpp"

#include <QObject>

class QObjectQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant( const QVariant & qVariant,
		Variant & o_variant,
		void * parent,
		const std::string & path ) const override
	{
		if ( static_cast<int>( qVariant.type() ) != static_cast<int>( QMetaType::QObjectStar ))
		{
			return false;
		}

		o_variant = qvariant_cast< QObject * >( qVariant );
		return true;
	}

	bool toQVariant( const Variant & variant, QVariant & o_qVariant ) const override
	{
		if (variant.typeIs< ObjectHandle >())
		{
			ObjectHandle provider;
			if (!variant.tryCast( provider ))
			{
				return false;
			}

			auto qObject = provider.getBase< QObject >();
			if (qObject != nullptr)
			{
				o_qVariant = QVariant::fromValue( qObject );
				return true;
			}
		}

		return false;
	}
};

#endif