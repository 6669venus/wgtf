#include "buttons_model_extension.hpp"

#include "core_data_model\i_item.hpp"
#include "helpers\qt_helpers.hpp"

QHash<int, QByteArray> ButtonsModelExtension::roleNames() const
{
    QHash<int, QByteArray> roles;
    registerRole(ButtonIconRole::role_, roles);
    registerRole(ButtonEnabledRole::role_, roles);
    registerRole(ButtonClickedRole::role_, roles);
    return roles;
}

QVariant ButtonsModelExtension::data(const QModelIndex & index, int role) const
{
    size_t roleId;
    if (!decodeRole(role, roleId))
    {
        return QVariant();
    }

    assert(index.isValid());
    IItem* item = reinterpret_cast<IItem *>(index.internalPointer());
    if (item == nullptr)
    {
        return QVariant();
    }

    unsigned int supportedRoles[] = { ButtonIconRole::roleId_, ButtonEnabledRole::roleId_, ButtonClickedRole::roleId_ };

    if (std::find(std::begin(supportedRoles), std::end(supportedRoles), roleId) != std::end(supportedRoles))
    {
        return QtHelpers::toQVariant(item->getData(index.column(), roleId));
    }

    return QVariant();
}

bool ButtonsModelExtension::setData(const QModelIndex & index, const QVariant & value, int role)
{
    size_t roleId;
    if (!decodeRole(role, roleId))
    {
        return false;
    }

    assert(index.isValid());
    IItem* item = reinterpret_cast<IItem *>(index.internalPointer());
    if (item == nullptr)
    {
        return false;
    }

    unsigned int supportedRoles[] = { ButtonIconRole::roleId_, ButtonEnabledRole::roleId_, ButtonClickedRole::roleId_ };

    if (std::find(std::begin(supportedRoles), std::end(supportedRoles), roleId) != std::end(supportedRoles))
    {
        return item->setData(index.column(), roleId, QtHelpers::toVariant(value));
    }

    return false;
}
