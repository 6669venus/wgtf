#ifndef TREE_EXTENSION_HPP
#define TREE_EXTENSION_HPP

#include "i_model_extension.hpp"
#include <QItemSelection>
#include <memory>

class TreeExtension : public IModelExtension
{
	Q_OBJECT

public:
	TreeExtension();
	virtual ~TreeExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

	void onLayoutAboutToBeChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) override;
	void onLayoutChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) override;
	void onRowsAboutToBeRemoved(
		const QModelIndex& parent, int first, int last ) override;
	void onRowsRemoved( 
		const QModelIndex & parent, int first, int last ) override;

	Q_INVOKABLE QItemSelection itemSelection( const QModelIndex & first, const QModelIndex & last ) const;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // TREE_EXTENSION_HPP