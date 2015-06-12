#include "bw_copyable.hpp"
#include "helpers/qt_helpers.hpp"
#include "copy_paste_system/i_copy_paste_manager.hpp"
#include <cassert>
#include <QtCore>
#include <QQmlEngine>

//==============================================================================
BWCopyable::BWCopyable( QObject * parent )
	: QObject( parent )
	, valueHint_( "" )
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

	auto data = QtHelpers::toQVariant( value);
	if (data_ != data)
	{
		data_ = data;
		emit dataPasted();
	}
	return true;
}


