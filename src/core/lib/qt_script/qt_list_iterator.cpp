#include "qt_list_iterator.hpp"

#include "data_model/i_list_model.hpp"
#include "data_model/i_item.hpp"
#include "data_model/i_item_role.hpp"

#include "qt_common/helpers/qt_helpers.hpp"

QtListIterator::QtListIterator( IListModel & listModel )
	: listModel_( listModel )
	, size_( listModel.size() )
{
	reset();
}

QtListIterator::~QtListIterator()
{

}

void QtListIterator::reset()
{
	currentIndex_ = 0;
	currentItem_ = nullptr;
}

bool QtListIterator::moveNext()
{
	if (currentIndex_ < size_)
	{
		currentItem_ = listModel_.item( currentIndex_++ );
		return true;
	}
	currentItem_ = nullptr;
	return false;
}

QVariant QtListIterator::getCurrent() const
{
	if (currentItem_ == nullptr)
	{
		return QVariant();
	}

	auto data = currentItem_->getData( 0, ValueRole::roleId_ );
	return QtHelpers::toQVariant( data );
}

void QtListIterator::setCurrent( QVariant & value )
{
	if (currentItem_ == nullptr)
	{
		return;
	}

	auto data = QtHelpers::toVariant( value );
	currentItem_->setData( 0, ValueRole::roleId_, data );
}