#include "bw_combobox.hpp"

#include "../helpers/qt_helpers.hpp"
#include "core_logging/logging.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_qt_common/models/extensions/value_extension.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_reflection/object_handle.hpp"


#include <QtCore>
#include <QQmlEngine>


//==============================================================================
BWComboBox::BWComboBox( QQuickItem * parent )
	: QQuickItem( parent )
{
	connect(
		this, &BWComboBox::chosenDisplayItemChanged,
		this, &BWComboBox::chosenDisplayItemChangedInternal );
	connect(
		this, &BWComboBox::childrenRectChanged,
		this, &BWComboBox::childrenRectChangedInternal );
	setHeight( childrenRect().height() );
}

//==============================================================================
void BWComboBox::childrenRectChangedInternal()
{
	setHeight( childrenRect().height() );
}

//==============================================================================
BWComboBox::~BWComboBox()
{
	disconnect(
		this, &BWComboBox::chosenDisplayItemChanged,
		this, &BWComboBox::chosenDisplayItemChangedInternal );
	disconnect(
		this, &BWComboBox::childrenRectChanged,
		this, &BWComboBox::childrenRectChangedInternal );
}

//==============================================================================
void BWComboBox::componentComplete()
{
	// Derived classes should call the base class method before adding their
	// own actions to perform at componentComplete.
	QQuickItem::componentComplete();

	QQuickItem * pThis = this;
	QQmlEngine * engine = qmlEngine( this );
	QUrl qurl = QtHelpers::resolveQmlPath( *engine, "qt_common/bw_combobox.qml" );
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
			this->chosenDisplayItemChangedInternal();
		}
	}
}

//==============================================================================
QVariant BWComboBox::getComboModel()
{
	return QVariant::fromValue( comboModel_.get() );
}


//==============================================================================
void BWComboBox::setComboModel( QVariant comboModel )
{
	auto oldModel = comboModel_.get();
	auto newModel = QtHelpers::toVariant( comboModel );
	if (newModel.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (newModel.tryCast( provider ))
		{
			auto listModel = provider.getBase< IListModel >();
			if (listModel != nullptr)
			{
				comboModel_ = std::unique_ptr< WGListModel >( new WGListModel() );
				comboModel_->registerExtension< ValueExtension >();
				comboModel_->source( listModel );
			}
		}
	}

	if (comboModel_.get() != oldModel)
	{
		emit comboModelChanged();
	}
}

//==============================================================================
QVariant BWComboBox::getChosenItem()
{
	return chosenItem_;
}

//==============================================================================
void BWComboBox::setChosenItem( QVariant chosenItem )
{
	chosenItem_ = chosenItem;
	emit chosenItemChanged();
	emit chosenDisplayItemChanged();
}

//==============================================================================
QString BWComboBox::getChosenDisplayString()
{
	if (comboModel_ != nullptr)
	{
		auto listModel = comboModel_->source();
		auto chosenData = QtHelpers::toVariant( chosenItem_ );
		//TODO: This is horribly inefficient for an extremely larger list model,
		//We should be able to look up the item much quicker than this.
		for (size_t i = 0; i < listModel->size(); ++i)
		{
			auto item = listModel->item( i );
			auto data = item->getData( 0, ValueRole::roleId_ );
			if (data == chosenData)
			{
				return item->getDisplayText( 0 );
			}
		}
		
	}
	return QString();
}

//==============================================================================
void BWComboBox::chosenDisplayItemChangedInternal()
{
	QQuickItem * comboText =
		QtHelpers::findChildByObjectName( this, "comboText" );
	if( comboText == NULL )
	{
		return;
	}
	comboText->setProperty( "text", getChosenDisplayString() );
}