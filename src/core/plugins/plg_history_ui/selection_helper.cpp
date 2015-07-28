#include "selection_helper.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "reflection/object_handle.hpp"
#include "variant/variant.hpp"
#include "variant/collection.hpp"
#include "history_selection_handler.hpp"


//==============================================================================
SelectionHelper::SelectionHelper( QObject * parent )
    : QObject( parent )
{
}


//==============================================================================
SelectionHelper::~SelectionHelper()
{
}


//==============================================================================
void SelectionHelper::source( SourceType* source )
{

	source_ = source;
}


//==============================================================================
const SelectionHelper::SourceType* SelectionHelper::source() const
{
	return source_;
}


//==============================================================================
QVariant SelectionHelper::getSource() const
{
	Variant variant = ObjectHandle( const_cast< SourceType* >( source_ ) );
	return QtHelpers::toQVariant( variant );
}


//==============================================================================
bool SelectionHelper::setSource( const QVariant& source )
{
	Variant variant = QtHelpers::toVariant( source );
	if (variant.typeIs< ObjectHandle >())
	{
		ObjectHandle provider;
		if (variant.tryCast( provider ))
		{
			auto selectionHandler = provider.getBase< SourceType >();
			if (selectionHandler != nullptr)
			{
				this->source( selectionHandler );
				return true;
			}
		}
	}

	return false;
}


//==============================================================================
void SelectionHelper::select( const QVariant& value )
{
	assert( source_ != nullptr );
	const Variant & variant = QtHelpers::toVariant( value );
	Collection selections;
	bool isOk = variant.tryCast( selections );
	if (!isOk)
	{
		assert( isOk );
		return;
	}
	source_->setSelection( selections );
}
