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

	Q_INVOKABLE QItemSelection itemSelection( const QModelIndex & first, const QModelIndex & last ) const;

	/**
	 *	Get an index with the given row.
	 *	@param index to be moved to row.
	 *	@param row the position to move the index.
	 *	@return new position.
	 */
	Q_INVOKABLE QModelIndex getRowToIndex( const QModelIndex & index,
		const int row ) const;

	/**
	 *	Get an row from the given index.
	 *	@param index from which to get the row.
	 *	@return row.
	 */
	Q_INVOKABLE int getIndexToRow( const QModelIndex & index ) const;
};

#endif // LIST_EXTENSION_HPP