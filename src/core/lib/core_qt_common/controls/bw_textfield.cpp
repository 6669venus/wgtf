#include "bw_textfield.hpp"

#include "core_logging/logging.hpp"

#include "../helpers/qt_helpers.hpp"
#include "core_string_utils/text_utilities.hpp"


#include <QtCore>
#include <QQmlEngine>
#include <cassert>

//==============================================================================
BWTextField::BWTextField( QQuickItem * parent )
	: QQuickItem( parent )
{
	connect(
		this, &BWTextField::textChanged,
		this, &BWTextField::textChangedInternal );
	connect(
		this, &BWTextField::childrenRectChanged,
		this, &BWTextField::childrenRectChangedInternal );
	setHeight( childrenRect().height() );
}

//==============================================================================
BWTextField::~BWTextField()
{
	disconnect(
		this, &BWTextField::textChanged,
		this, &BWTextField::textChangedInternal );
	disconnect(
		this, &BWTextField::childrenRectChanged,
		this, &BWTextField::childrenRectChangedInternal );
}

//==============================================================================
void BWTextField::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();

	QQuickItem * pThis = this;
	QQmlEngine * engine = qmlEngine( this );
	QUrl qurl = QtHelpers::resolveQmlPath( *engine, "qt_common/bw_textfield.qml" );
	if (!qurl.isValid())
	{
		NGT_ERROR_MSG( "Invalid QUrl\n" );
		return;
	}
	QQmlComponent component( engine, qurl, this );
	assert( !component.isLoading() );
	if (component.isError() || !component.isReady())
	{
		NGT_ERROR_MSG( "Error loading component: %s\n",
			qPrintable( component.errorString() ) );
	}
	else
	{
		QQuickItem * qmlView = qobject_cast< QQuickItem* >(
			component.create( qmlContext( this ) ) );

		if (qmlView == nullptr)
		{
			NGT_ERROR_MSG( "Failed to create component\n" );
		}
		else
		{
			qmlView->setParent( this );
			qmlView->setParentItem( this );
		}
	}
}


//==============================================================================
QVariant BWTextField::getText()
{
	return QVariant( text_ );
}


//==============================================================================
void BWTextField::setText( QVariant displayText )
{
	const char * typeName = displayText.typeName();
	QString stringValue = displayText.toString();
	if (text_ == stringValue)
	{
		return;
	}
	text_ = stringValue;
	emit textChanged();
}


//==============================================================================
void BWTextField::textChangedInternal()
{
	QQuickItem * textField =
		QtHelpers::findChildByObjectName( this, "textField" );
	if (textField != NULL)
	{
		textField->setProperty( "text", QVariant( text_ ) );
	}
	QQuickItem * textInput =
		QtHelpers::findChildByObjectName( this, "textInput" );
	if (textInput != NULL)
	{
		textInput->setProperty( "text", QVariant( text_ ) );
	}
}


//==============================================================================
void BWTextField::childrenRectChangedInternal()
{
	setHeight( childrenRect().height() );
}


//==============================================================================
QVariantMap BWTextField::upPressed(
	QVariant textFieldWrapper, QVariant cursorPositionWrapper )
{
	QQuickItem * textField = 
		qvariant_cast< QQuickItem * >( textFieldWrapper );
	assert( textField != NULL );
	int cursorPosition = cursorPositionWrapper.toInt();
	QVariant textWrapper =
		textField->property( "text" );
	QString text = qvariant_cast< QString >( textWrapper );

	const QLocale & cLocale = QLocale::c();

	assert( text.length() < 1024 );
	std::wstring newValue;
	newValue.resize( text.length() + 1, 0 );
	text.toWCharArray( &newValue.front() );

	TextUtilities::incrementNumber(
		newValue, cursorPosition,
		cLocale.decimalPoint().unicode() );

	QVariantMap result;
	result[ "cursorPosition" ] = cursorPosition;
	result[ "text" ] = QString::fromWCharArray( newValue.c_str() );

	return result;
}


//==============================================================================
QVariantMap BWTextField::downPressed(
	QVariant textFieldWrapper, QVariant cursorPositionWrapper )
{
	QQuickItem * textField = 
		qvariant_cast< QQuickItem * >( textFieldWrapper );
	assert( textField != NULL );
	int cursorPosition = cursorPositionWrapper.toInt();

	QVariant textWrapper =
		textField->property( "text" );
	QString text = qvariant_cast< QString >( textWrapper );

	const QLocale & cLocale = QLocale::c();

	assert( text.length() < 1024 );
	std::wstring newValue;
	newValue.resize( text.length() + 1, 0 );
	text.toWCharArray( &newValue.front() );

	TextUtilities::decrementNumber(
		newValue, cursorPosition,
		cLocale.decimalPoint().unicode() );

	QVariantMap result;
	result[ "cursorPosition" ] = cursorPosition;
	result[ "text" ] = QString::fromWCharArray( newValue.c_str() );

	return result;
}