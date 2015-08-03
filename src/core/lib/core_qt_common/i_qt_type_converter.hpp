#ifndef I_QT_TYPE_CONVERTER_HPP
#define I_QT_TYPE_CONVERTER_HPP
#include "wg_types/binary_block.hpp"
#include <core_variant/variant.hpp>
#include <QVariant>


class ObjectHandle;

Q_DECLARE_METATYPE( std::shared_ptr< BinaryBlock > );

/**
 *	Interface for converting custom C++ types to/from QVariant.
 */
class IQtTypeConverter
{
public:
	virtual bool toVariant( const QVariant & qVariant, Variant & o_variant ) const = 0;
	virtual bool toQVariant( const Variant & variant, QVariant & o_qVariant ) const = 0;
	virtual bool toQVariant( const ObjectHandle & object,
		QVariant & o_qVariant ) const
	{ return false; };
};

template< typename T >
class GenericQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant( const QVariant & qVariant, Variant & o_variant ) const override
	{
		int typeId = qVariant.type();
		if (typeId == QVariant::UserType)
		{
			typeId = qVariant.userType();
		}

		if (typeId != qMetaTypeId<T>())
		{
			return false;
		}

		o_variant = qVariant.value<T>();
		return true;
	}

	bool toQVariant( const Variant & variant, QVariant & o_qVariant ) const override
	{
		if (variant.typeIs< T >() == false)
		{
			return false;
		}
		T tmp;
		if (variant.tryCast( tmp ))
		{
			o_qVariant = QVariant::fromValue( tmp );
		}
		return true;
	}
};

#endif