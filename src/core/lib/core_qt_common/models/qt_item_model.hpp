#ifndef QT_ITEM_MODEL_HPP
#define QT_ITEM_MODEL_HPP

#include <QAbstractItemModel>
#include <memory>

class AbstractItemModel;
class AbstractListModel;
class AbstractTreeModel;
class AbstractTableModel;

class QtItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	QtItemModel( AbstractItemModel & source );
	virtual ~QtItemModel();

	AbstractItemModel & source() const;

	Q_INVOKABLE QObject * item( int row, int column, const QObject * parent ) const;

	Q_INVOKABLE int rowCount( const QObject * parent ) const;
	Q_INVOKABLE int columnCount( const QObject * parent ) const;

	// QAbstractItemModel
	Q_INVOKABLE QModelIndex index( int row, int column, const QModelIndex &parent ) const override;
	Q_INVOKABLE QModelIndex parent( const QModelIndex &child ) const override;

	Q_INVOKABLE int rowCount( const QModelIndex &parent ) const override;
	Q_INVOKABLE int columnCount( const QModelIndex &parent ) const override;

	Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
	Q_INVOKABLE bool setData( const QModelIndex &index, const QVariant &value, int role ) override;

	Q_INVOKABLE QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
	bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) override;

private:
	struct Impl;
	std::unique_ptr< Impl > impl_;
};

class QtListModel : public QtItemModel
{
	Q_OBJECT

public:
	QtListModel( AbstractListModel & source );

	AbstractListModel & source() const;

	Q_INVOKABLE QObject * item( int row ) const;
	Q_INVOKABLE int count() const;
};

class QtTreeModel : public QtItemModel
{
	Q_OBJECT

public:
	QtTreeModel( AbstractTreeModel & source );

	AbstractTreeModel & source() const;

	Q_INVOKABLE QObject * item( int row, QObject * parent ) const;
	Q_INVOKABLE int count( QObject * parent ) const ;
};

class QtTableModel : public QtItemModel
{
	Q_OBJECT

public:
	QtTableModel( AbstractTableModel & source );

	AbstractTableModel & source() const;

	Q_INVOKABLE QObject * item( int row, int column ) const;
	Q_INVOKABLE int rowCount() const;
	Q_INVOKABLE int columnCount() const;

private:
	using QtItemModel::rowCount;
	using QtItemModel::columnCount;
};

#endif//QT_ITEM_MODEL_HPP