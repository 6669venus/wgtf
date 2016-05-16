#ifndef LIST_EXTENSION_HPP
#define LIST_EXTENSION_HPP

#include "i_model_extension.hpp"

#include <QItemSelection>

class ListExtension : public IModelExtension
{
	Q_OBJECT

public:
	ListExtension();
	virtual ~ListExtension();

	QHash< int, QByteArray > roleNames() const override;

	/**
	 *	Convert first and last index into a selection range.
	 *	@param first start of selected area.
	 *	@param last end of selected area. End can be before begin.
	 *	@return area covered inbetween first and last.
	 */
	Q_INVOKABLE QItemSelection itemSelection( const QModelIndex & first, const QModelIndex & last ) const;
	Q_INVOKABLE QModelIndex incIndex( const QModelIndex & index ) const;
	Q_INVOKABLE QModelIndex decIndex( const QModelIndex & index ) const;
	Q_INVOKABLE int indexToRow( const QModelIndex & index ) const;
};

#endif // LIST_EXTENSION_HPP