#include "bw_checkbox.hpp"

#include "logging/logging.hpp"
#
#include "../helpers/qt_helpers.hpp"

#include <cassert>
#include <QtCore>
#include <QQmlEngine>


//==============================================================================
BWCheckBox::BWCheckBox( QQuickItem * parent )
	: QQuickItem( parent )
{
	connections_.push_back(
		connect(
			this, &BWCheckBox::checkedChanged,
			this, &BWCheckBox::checkedChangedInternal ) );
	connections_.push_back( 
		connect(
			this, &BWCheckBox::childrenRectChanged,
			this, &BWCheckBox::childrenRectChangedInternal ) );
	setHeight( childrenRect().height() );
}

//==============================================================================
BWCheckBox::~BWCheckBox()
{
	auto it = connections_.begin();
	auto itEnd = connections_.end();
	for( ; it != itEnd; ++it )
	{
		disconnect( *it );
	}
}

//==============================================================================
void BWCheckBox::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();

	QQuickItem * pThis = this;
	QQmlEngine * engine = qmlEngine( this );
	QUrl qurl = QUrl( "qrc:///qt_common/bw_checkbox.qml" );
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
			this->checkedChangedInternal();
		}
	}
}


//==============================================================================
QVariant BWCheckBox::getChecked()
{
	return QVariant( checked_ );
}


//==============================================================================
void BWCheckBox::setChecked( QVariant checked )
{
	const char * typeName = checked.typeName();
	bool boolValue = checked.toBool();
	if (checked_ == boolValue)
	{
		return;
	}
	checked_ = boolValue;
	emit checkedChanged();
}


//==============================================================================
void BWCheckBox::checkedChangedInternal()
{
	QQuickItem * checkBox =
		QtHelpers::findChildByObjectName( this, "childCheckBox" );
	if (checkBox != NULL)
	{
		checkBox->setProperty(
			"checked", QVariant( checked_ ? Qt::Checked : Qt::Unchecked ) );
		checkBox->setProperty( "text", QVariant( checked_ ) );
	}
}


//==============================================================================
void BWCheckBox::childrenRectChangedInternal()
{
	setHeight( childrenRect().height() );
}