#include "selection_helper.hpp"
#include "qt_common/helpers/qt_helpers.hpp"
#include "reflection/object_handle.hpp"
#include "variant/variant.hpp"
#include "history_selection_handler.hpp"
#include <QVariant>
#include <QPersistentModelIndex>


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
void SelectionHelper::select( const QList<QVariant>& selectionList )
{
	assert( source_ != nullptr );
	std::vector<unsigned int> selections;
	for (auto & selection : selectionList)
	{
		assert( selection.canConvert<QModelIndex>() );
		QModelIndex index = selection.toModelIndex();
		selections.push_back( index.row() );
	}
	source_->setSelection( selections );
}
