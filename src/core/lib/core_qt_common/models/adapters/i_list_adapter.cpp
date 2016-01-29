#include "i_list_adapter.hpp"

#include <cassert>

IListAdapter::~IListAdapter()
{
}

void IListAdapter::reset()
{
	cachedAdaptedIndices_.clear();
}

void IListAdapter::connect()
{
	auto model = this->model();
	if (model == nullptr)
	{
		return;
	}

	connections_ += QObject::connect( 
		model, &QAbstractItemModel::dataChanged,
		this, &IListAdapter::onParentDataChanged );
	connections_ += QObject::connect( 
		model, &QAbstractItemModel::layoutAboutToBeChanged,
		this, &IListAdapter::onParentLayoutAboutToBeChanged );
	connections_ += QObject::connect( 
		model, &QAbstractItemModel::layoutChanged,
		this, &IListAdapter::onParentLayoutChanged );
	connections_ += QObject::connect(
		model, &QAbstractItemModel::rowsAboutToBeInserted,
		this, &IListAdapter::onParentRowsAboutToBeInserted );
	connections_ += QObject::connect(
		model, &QAbstractItemModel::rowsInserted,
		this, &IListAdapter::onParentRowsInserted);
	connections_ += QObject::connect(
		model, &QAbstractItemModel::rowsAboutToBeRemoved,
		this, &IListAdapter::onParentRowsAboutToBeRemoved );
	connections_ += QObject::connect(
		model, &QAbstractItemModel::rowsRemoved,
		this, &IListAdapter::onParentRowsRemoved);
}

void IListAdapter::disconnect()
{
	connections_.reset();
}

QHash< int, QByteArray > IListAdapter::roleNames() const
{
	auto model = this->model();
	if (model == nullptr)
	{
		return QHash< int, QByteArray >();
	}

	return model->roleNames();
}

QModelIndex IListAdapter::index(int row, int column, const QModelIndex &parent) const
{
	auto cachedIndex = cachedAdaptedIndex( row, column, parent );
	return createIndex( row, column, cachedIndex.internalPointer() );
}

QVariant IListAdapter::data(const QModelIndex &index, int role) const
{
	auto model = this->model();
	if (model == nullptr)
	{
		return QVariant( QVariant::Invalid );
	}

	auto cachedIndex = cachedAdaptedIndex( 
		index.row(), index.column(), index.parent() );
	return model->data( cachedIndex, role );
}

bool IListAdapter::setData(const QModelIndex &index, const QVariant &value, int role)
{
	auto model = this->model();
	if (model == nullptr)
	{
		return false;
	}
	
	auto cachedIndex = cachedAdaptedIndex( 
		index.row(), index.column(), index.parent() );
	return model->setData( cachedIndex, value, role );
}

QModelIndex IListAdapter::cachedAdaptedIndex(int row, int column, const QModelIndex &parent) const
{
	assert( column == 0 && !parent.isValid() );
	auto it = cachedAdaptedIndices_.find( row );
	if (it != cachedAdaptedIndices_.end())
	{
		return it->second;
	}

	auto index = adaptedIndex( row, column, parent );
	cachedAdaptedIndices_[row] = index;
	return index;
}