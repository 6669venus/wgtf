#ifndef I_COMBOBOX_LIST_MODEL_HPP
#define I_COMBOBOX_LIST_MODEL_HPP
#include "i_list_model.hpp"

/**
 *	Interface for accessing data as a "list" for WGDropDownBox control.
 */
class IComboBoxListModel
    : public IListModel
{
public:
	virtual ~IComboBoxListModel() {}

	virtual IItem * findItemByData(const Variant & data) const { return nullptr; }
};

#endif // I_COMBOBOX_LIST_MODEL_HPP
