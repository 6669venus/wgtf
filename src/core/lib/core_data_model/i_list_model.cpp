#include "i_list_model.hpp"
#include "core_variant/variant.hpp"


bool IListModel::empty() const
{
	return this->size() == 0;
}


Variant IListModel::getData( int column, size_t roleId ) const
{
	return Variant();
}


bool IListModel::setData( int column, size_t roleId, const Variant & data )
{
	return false;
}
