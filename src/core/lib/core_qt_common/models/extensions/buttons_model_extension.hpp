#ifndef BUTTONS_MODEL_EXTENSION_HPP
#define BUTTONS_MODEL_EXTENSION_HPP

#include "i_model_extension.hpp"

namespace wgt
{

class ButtonsModelExtension: public IModelExtension
{
public:
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

} // namespace wgt

#endif