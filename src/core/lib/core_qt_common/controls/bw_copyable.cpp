#include "bw_copyable.hpp"
#include "helpers/qt_helpers.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"
#include <cassert>
#include <QtCore>
#include <QQmlEngine>

//==============================================================================
BWCopyable::BWCopyable( QObject * parent )
	: QObject( parent )
	, valueHint_( "" )
	, bPasted_( false )
{
}


//==============================================================================
BWCopyable::~BWCopyable()
{
}


//==============================================================================
void BWCopyable::setValue( const QVariant & data )
{
	if (data_ == data)
	{
		return;
	}
	data_ = data;
}

//==============================================================================
void BWCopyable::setValueHint( const QString & hint )
{
	if(hint.isEmpty() || hint.isNull())
	{
		return;
	}
	valueHint_ = hint.toUtf8().constData();
}

//==============================================================================
const char * BWCopyable::getDataHint() const
{
	return valueHint_.c_str();
}

//==============================================================================
const Variant & BWCopyable::getData()
{
	emit dataCopied();
	value_ = QtHelpers::toVariant( data_ );
	return value_;
}


//==============================================================================
bool BWCopyable::setData( const Variant& value )
{

	auto data = QtHelpers::toQVariant( value, this );
	if (data_ != data)
	{
		data_ = data;
		bPasted_ = true;
		emit dataPasted();
		return bPasted_;
	}
	return false;
}


