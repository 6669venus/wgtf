#ifndef I_MODEL_EXTENSION_HPP
#define I_MODEL_EXTENSION_HPP

#include "core_data_model/i_item_role.hpp"
#include "core_qt_common/models/role_provider.hpp"
#include "core_qt_common/qt_new_handler.hpp"

#include <cassert>

#include <QHash>
#include <QPersistentModelIndex>
#include <QVariant>

class QAbstractItemModel;
class IQtFramework;

class IModelExtension : public QObject
					  , public RoleProvider
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	IModelExtension() : model_( nullptr ) {}
	virtual ~IModelExtension() {}

	void init( QAbstractItemModel * model, IQtFramework* qtFramework )
	{
		assert( model_ == nullptr );
		model_ = model;
		qtFramework_ = qtFramework;
	}

	virtual QHash< int, QByteArray > roleNames() const = 0;

	virtual QVariant data( const QModelIndex &index, int role ) const { return QVariant::Invalid; }
	virtual bool setData( const QModelIndex &index, const QVariant &value, int role ) { return false; }

	virtual void saveStates( const char * modelUniqueName ) {}
	virtual void loadStates( const char * modelUniqueName ) {}

public slots:
	virtual void onDataAboutToBeChanged( 
		const QModelIndex &index, int role, const QVariant &value ) {}
	virtual void onDataChanged( 
		const QModelIndex &index, int role, const QVariant &value ) {}
	virtual void onLayoutAboutToBeChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) {}
	virtual void onLayoutChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) {}
	virtual void onRowsAboutToBeInserted( 
		const QModelIndex& parent, int first, int last ) {}
	virtual void onRowsInserted(
		const QModelIndex & parent, int first, int last ) {}
	virtual void onRowsAboutToBeRemoved(
		const QModelIndex& parent, int first, int last ) {}
	virtual void onRowsRemoved( 
		const QModelIndex & parent, int first, int last ) {}

protected:
	QAbstractItemModel * model_;
	IQtFramework * qtFramework_;
};

#endif // I_MODEL_EXTENSION_HPP