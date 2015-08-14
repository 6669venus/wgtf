#ifndef REFLECTED_ENUM_MODEL_HPP
#define REFLECTED_ENUM_MODEL_HPP

#include "core_data_model/i_combobox_list_model.hpp"
#include <vector>

class PropertyAccessor;
class MetaEnumObj;

class ReflectedEnumModel : public IComboBoxListModel
{
public:
	ReflectedEnumModel( const PropertyAccessor &, const MetaEnumObj * enumObj );
	virtual ~ReflectedEnumModel();

	IItem * item( size_t index ) const override;
	size_t index( const IItem * item ) const override;
	IItem * findItemByData(const Variant & data) const override;

	bool empty() const override;
	size_t size() const override;

private:
	std::vector< IItem * > items_;
};

#endif // REFLECTED_ENUM_MODEL_HPP