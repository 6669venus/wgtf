#ifndef LIST_EXTENSION_HPP
#define LIST_EXTENSION_HPP

#include "i_model_extension.hpp"

#include <QItemSelection>

namespace wgt
{
class ListExtension : public IModelExtension
{
	Q_OBJECT

public:
	ListExtension();
	virtual ~ListExtension();

	QHash< int, QByteArray > roleNames() const override;

	Q_INVOKABLE QItemSelection itemSelection( const QModelIndex & first, const QModelIndex & last ) const;
};

} // end namespace wgt
#endif // LIST_EXTENSION_HPP
