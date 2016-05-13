#ifndef BUTTONS_MODEL_HPP
#define BUTTONS_MODEL_HPP

#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"

class ButtonItem: public IItem
{
public:
    ButtonItem(bool isEnabled, const std::string& iconUrl, const std::function<void(void)>& clickCallback);

    const char * getDisplayText(int column) const override;
    ThumbnailData getThumbnail(int column) const override;
    Variant getData(int column, size_t roleId) const override;
    bool setData(int column, size_t roleId, const Variant & data) override;

private:
    bool isEnabled;
    std::string iconUrl;
    std::function<void(void)> clickCallback;
};

class ButtonsModel: public IListModel
{
public:
    ButtonsModel(std::vector<ButtonItem> && buttons);

    IItem * item(size_t index) const override;
    size_t index(const IItem * item) const override;
    bool empty() const override;
    size_t size() const override;
    int columnCount() const override;

private:
    // "mutable" is the only one way to return non-const pointer from ::item() method. 
    // I don't want to store items in heap.
    mutable std::vector<ButtonItem> buttons;
};

#endif