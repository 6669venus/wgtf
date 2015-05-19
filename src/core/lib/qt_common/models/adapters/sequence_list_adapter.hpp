#ifndef SEQUENCE_LIST_ADAPTER_HPP
#define SEQUENCE_LIST_ADAPTER_HPP

#include "i_list_adapter.hpp"
#include "qt_common/qt_new_handler.hpp"

class SequenceListAdapter : public IListAdapter
{
	Q_OBJECT
		Q_PROPERTY(QVariant model READ getModel WRITE setModel)
		Q_PROPERTY(QList< int > sequence READ getSequence WRITE setSequence)

	DECLARE_QT_MEMORY_HANDLER

public:
	SequenceListAdapter();
	virtual ~SequenceListAdapter();

	QAbstractItemModel * model() const;

	QModelIndex adaptedIndex(int row, int column, const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent) const;

private:
	void onParentDataChanged(const QModelIndex &topLeft, 
		const QModelIndex &bottomRight, const QVector<int> &roles);

	QVariant getModel() const;
	void setModel( const QVariant &model );

	QList< int > getSequence();
	void setSequence( const QList< int > &sequence );

private:
	QAbstractListModel *model_;
	QList< int > sequence_;
};

#endif // SEQUENCE_LIST_ADAPTER_HPP

