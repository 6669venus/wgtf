#include "buttons_model.hpp"

#include "core_data_model/i_item_role.hpp"

ButtonItem::ButtonItem(bool isEnabled_, const std::string& iconUrl_, const std::function<void(void)>& clickCallback_)
    : isEnabled(isEnabled_)
    , iconUri(iconUrl_)
    , clickCallback(clickCallback_)
{
}

const char * ButtonItem::getDisplayText(int column) const
{
    return nullptr;
}

ThumbnailData ButtonItem::getThumbnail(int column) const
{
    return nullptr;
}

Variant ButtonItem::getData(int column, size_t roleId) const
{
    if (roleId == ButtonIconRole::roleId_)
    {
        return Variant(iconUri);
    }
    else if (roleId == ButtonEnabledRole::roleId_)
    {
        return Variant(isEnabled);
    }

    return Variant();
}

bool ButtonItem::setData(int column, size_t roleId, const Variant & data)
{
    if (roleId == ButtonClickedRole::roleId_)
    {
        clickCallback();
        return true;
    }

    return false;
}

ButtonsModel::ButtonsModel(std::vector<ButtonItem> && buttons_)
    : buttons(std::move(buttons_))
{
}

bool ButtonsModel::isEnabled(size_t index) const
{
    ButtonItem* button = static_cast<ButtonItem*>(item(index));
    return button->isEnabled;
}

void ButtonsModel::setEnabled(size_t index, bool isEnabled)
{
    ButtonItem* button = static_cast<ButtonItem*>(item(index));
    if (button->isEnabled == isEnabled)
    {
        return;
    }
    notifyPreDataChanged(button, 0, ButtonEnabledRole::roleId_, button->isEnabled);
    button->isEnabled = isEnabled;
    notifyPostDataChanged(button, 0, ButtonEnabledRole::roleId_, isEnabled);
}

const std::string& ButtonsModel::getIconUri(size_t index) const
{
    ButtonItem* button = static_cast<ButtonItem*>(item(index));
    return button->iconUri;
}

void ButtonsModel::setIconUri(size_t index, const std::string& iconUri)
{
    ButtonItem* button = static_cast<ButtonItem*>(item(index));
    if (button->iconUri == iconUri)
    {
        return;
    }
    notifyPreDataChanged(button, 0, ButtonIconRole::roleId_, button->iconUri);
    button->iconUri = iconUri;
    notifyPostDataChanged(button, 0, ButtonIconRole::roleId_, iconUri);
}

IItem * ButtonsModel::item(size_t index) const
{
    assert(index < buttons.size());
    return &buttons[index];
}

size_t ButtonsModel::index(const IItem * item) const
{
    auto iter = std::find_if(buttons.begin(), buttons.end(), [item](const ButtonItem& buttonItem)
    {
        return item == &buttonItem;
    });

    assert(iter != buttons.end());
    return std::distance(buttons.begin(), iter);
}

bool ButtonsModel::empty() const
{
    return buttons.empty();
}

size_t ButtonsModel::size() const
{
    return buttons.size();
}

int ButtonsModel::columnCount() const
{
    return 1;
}