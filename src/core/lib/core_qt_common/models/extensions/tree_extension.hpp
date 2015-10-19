#ifndef TREE_EXTENSION_HPP
#define TREE_EXTENSION_HPP

#include "i_model_extension.hpp"
#include <memory>

class TreeExtension : public IModelExtension
{
	Q_OBJECT

	Q_PROPERTY( QVariant	currentIndex
				READ		getCurrentIndex
				WRITE		setCurrentIndex
				NOTIFY		currentIndexChanged )

	Q_PROPERTY( bool		blockSelection
				READ		getBlockSelection
				WRITE		setBlockSelection
				NOTIFY		blockSelectionChanged )

	Q_PROPERTY( QObject *	selectionExtension
				READ		getSelectionExtension
				WRITE		setSelectionExtension
				NOTIFY		selectionExtensionChanged )

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

	void saveStates( const char * modelUniqueName ) override;
	void loadStates( const char * modelUniqueName ) override;

	Q_INVOKABLE void moveUp();
	Q_INVOKABLE void moveDown();
	Q_INVOKABLE void moveLeft();
	Q_INVOKABLE void moveRight();
	Q_INVOKABLE void selectItem();

signals:
	void currentIndexChanged();
	void selectionExtensionChanged();
	void blockSelectionChanged();

private:
	QVariant getCurrentIndex() const;
	void setCurrentIndex( const QVariant& index );

	bool getBlockSelection() const;
	void setBlockSelection( bool blockSelection );

	QObject * getSelectionExtension() const;
	void setSelectionExtension( QObject * selectionExtension );

	void handleCurrentIndexChanged();

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // TREE_EXTENSION_HPP