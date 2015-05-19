#ifndef BW_CORE_QT_TYPE_CONVERTERS_HPP
#define BW_CORE_QT_TYPE_CONVERTERS_HPP

#include "qt_common/string_qt_type_converter.hpp"
#include "cstdmf/bw_string.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

class BWStringQtTypeConverter : public StringQtTypeConverter
{
public:
	bool toQVariant( const Variant & variant, QVariant & o_qVariant ) const override
	{
		if (variant.typeIs< BW::string >())
		{
			BW::string value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QString( value.c_str() );
			return true;
		}

		if (variant.typeIs< BW::wstring >())
		{
			BW::wstring value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QString::fromWCharArray( value.c_str() );
			return true;
		}

		return false;
	}
};

class VectorQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant( const QVariant & qVariant, Variant & o_variant ) const override
	{
		if (qVariant.type() == QVariant::Vector2D)
		{
			auto vectorData = qVariant.value<QVector2D>();
			o_variant = BW::Vector2( 
				vectorData.x(), vectorData.y() );
			return true;
		}

		if (qVariant.type() == QVariant::Vector3D)
		{
			auto vectorData = qVariant.value<QVector3D>();
			o_variant = BW::Vector3( 
				vectorData.x(), vectorData.y(), vectorData.z() );
			return true;
		}

		if (qVariant.type() == QVariant::Vector4D)
		{
			auto vectorData = qVariant.value<QVector4D>();
			o_variant = BW::Vector4( 
				vectorData.x(), vectorData.y(), vectorData.z(), vectorData.w() );
			return true;
		}

		return false;
	}

	bool toQVariant( const Variant & variant, QVariant & o_qVariant ) const override
	{
		if (variant.typeIs< BW::Vector2 >())
		{
			BW::Vector2 value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QVariant::fromValue( 
				QVector2D( value.x, value.y ) );
			return true;
		}

		if (variant.typeIs< BW::Vector3 >())
		{
			BW::Vector3 value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QVariant::fromValue( 
				QVector3D( value.x, value.y, value.z ) );
			return true;
		}

		if (variant.typeIs< BW::Vector4 >())
		{
			BW::Vector4 value;
			if (!variant.tryCast( value ))
			{
				return false;
			}

			o_qVariant = QVariant::fromValue( 
				QVector4D( value.x, value.y, value.z, value.w ) );
			return true;
		}

		return false;
	}
};

#endif