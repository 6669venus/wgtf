#ifndef BUTTONS_MODEL_EXTENSION_HPP
#define BUTTONS_MODEL_EXTENSION_HPP

#include "i_model_extension.hpp"

class ButtonsModelExtension: public IModelExtension
{
public:
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

#endif