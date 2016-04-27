#pragma once
#ifndef _REFLECTED_ENUM_MODEL_NEW_HPP
#define _REFLECTED_ENUM_MODEL_NEW_HPP

#include "core_data_model/abstract_item_model.hpp"
#include <vector>

class PropertyAccessor;
class MetaEnumObj;

class ReflectedEnumModelNew : public AbstractListModel
{
public:
	ReflectedEnumModelNew( const PropertyAccessor &, const MetaEnumObj * enumObj );
	virtual ~ReflectedEnumModelNew();

	AbstractItem * item( int row ) const override;
	int index( const AbstractItem * item ) const override;

	int rowCount() const override;
	int columnCount() const override;

private:
	std::vector< AbstractItem * > items_;
};

#endif // _REFLECTED_ENUM_MODEL_NEW_HPP