#include "data_change_notifier.hpp"

#include "core_qt_common/helpers/qt_helpers.hpp"
#include "core_qt_common/models/qt_model_helpers.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"


DataChangeNotifier::DataChangeNotifier()
	: QObject()
	, source_( nullptr )
{
}


DataChangeNotifier::~DataChangeNotifier()
{
	if (source_ != nullptr)
	{
		source_->onPreDataChanged().remove< DataChangeNotifier,
			&DataChangeNotifier::onPreDataChanged >(this);
		source_->onPostDataChanged().remove< DataChangeNotifier,
			&DataChangeNotifier::onPostDataChanged >(this);
		source_->onPreNotifierDestroyed().remove< DataChangeNotifier,
			&DataChangeNotifier::onPreNofifierDestroyed >(this);
	}
}


void DataChangeNotifier::source( SourceType* source )
{
	if (source_ != nullptr)
	{
		source_->onPreDataChanged().remove< DataChangeNotifier,
			&DataChangeNotifier::onPreDataChanged >( this );
		source_->onPostDataChanged().remove< DataChangeNotifier,
			&DataChangeNotifier::onPostDataChanged >( this );
		source_->onPreNotifierDestroyed().remove< DataChangeNotifier,
			&DataChangeNotifier::onPreNofifierDestroyed >( this );
	}

	source_ = source;
	emit sourceChanged();

	if (source_ != nullptr)
	{
		source_->onPreDataChanged().add< DataChangeNotifier,
			&DataChangeNotifier::onPreDataChanged >( this );
		source_->onPostDataChanged().add< DataChangeNotifier,
			&DataChangeNotifier::onPostDataChanged >( this );
		source_->onPreNotifierDestroyed().add< DataChangeNotifier,
			&DataChangeNotifier::onPreNofifierDestroyed >(this);
	}
}


const DataChangeNotifier::SourceType* DataChangeNotifier::source() const
{
	return source_;
}


QVariant DataChangeNotifier::getSource() const
{
	Variant variant = ObjectHandle( const_cast< SourceType* >( source_ ) );
	return QtHelpers::toQVariant( variant );
}


bool DataChangeNotifier::setSource( const QVariant& source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto valueChangeNotifier = provider.getBase< SourceType >();
			if (valueChangeNotifier != nullptr)
			{
				this->source( valueChangeNotifier );
				return true;
			}
		}
	}

	return false;
}


QVariant DataChangeNotifier::getData() const
{
	assert( source_ != nullptr );
	return QtHelpers::toQVariant( source_->variantValue() );
}


bool DataChangeNotifier::setData( const QVariant& value )
{
	assert( source_ != nullptr );
	return source_->variantValue( QtHelpers::toVariant( value ) );
}


void DataChangeNotifier::onPreDataChanged( const SourceType* sender,
	const SourceType::PreDataChangedArgs& args )
{
	assert( source_ != nullptr );
	assert( sender == source_ );
	emit dataAboutToBeChanged();
}


void DataChangeNotifier::onPostDataChanged( const SourceType* sender,
	const SourceType::PostDataChangedArgs& args )
{
	assert( source_ != nullptr );
	assert( sender == source_ );
	emit dataChanged();
}

void DataChangeNotifier::onPreNofifierDestroyed(const SourceType* sender,
	const SourceType::PreNotifierDestroyedArgs& args)
{
	source_ = nullptr;
}

