#ifndef QT_LIST_MODEL_HPP
#define QT_LIST_MODEL_HPP

#include <QAbstractItemModel>
#include <QQmlListProperty>

#include "data_model/i_list_model.hpp"
#include "qt_common/qt_new_handler.hpp"
#include "qt_model_macros.hpp"


#include <memory>


class IModelExtension;


/**
 *	Qt interface for accessing data in an IListModel.
 *	Note that QAbstractListModel is supposed to only be used for
 *	one-dimensional lists (only has column 0).
 *	But IListModel can have any number of columns, so QtListModel must support
 *	multiple columns through the use of a ColumnExtension.
 */
class QtListModel : public QAbstractListModel
{
	Q_OBJECT

	Q_PROPERTY( QVariant source
		READ getSource
		WRITE setSource
		NOTIFY sourceChanged )
	Q_PROPERTY( QQmlListProperty< IModelExtension > extensions 
		READ getExtensions )
	Q_CLASSINFO( "DefaultProperty", "extensions" )

	DECLARE_QT_MEMORY_HANDLER

public:
	QtListModel();
	virtual ~QtListModel();

	void source( IListModel * source );
	const IListModel* source() const;

	Q_INVOKABLE bool canClear() const;
	Q_INVOKABLE void clear();

	template< typename T >
	void registerExtension()
	{
		auto extension = new T();
		extension->setParent( this );
		registerExtension( extension );
	}
	
private:
	void registerExtension( IModelExtension * extension );

	QHash< int, QByteArray > roleNames() const Q_DECL_OVERRIDE;

	QModelIndex index( int row,
		int column,
		const QModelIndex &parent ) const Q_DECL_OVERRIDE;

	int rowCount( const QModelIndex &parent ) const Q_DECL_OVERRIDE;
	int columnCount( const QModelIndex &parent ) const Q_DECL_OVERRIDE;

	QVariant data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;
	bool setData( const QModelIndex& index,
		const QVariant& value,
		int role ) Q_DECL_OVERRIDE;

	QVariant getSource() const;
	void setSource( const QVariant & source );

	QQmlListProperty< IModelExtension > getExtensions() const;

	static void appendExtension( 
		QQmlListProperty< IModelExtension > * property, 
		IModelExtension * value );
	static IModelExtension * extensionAt( 
		QQmlListProperty< IModelExtension > * property, 
		int index );
	static void clearExtensions( 
		QQmlListProperty< IModelExtension > * property );
	static int countExtensions( 
		QQmlListProperty< IModelExtension > * property );

signals:
	void sourceChanged();

private:
	EVENT_DECL( IListModel )
	EMIT_DECL

signals:
	void itemDataAboutToBeChanged( const QModelIndex &index, int role, const QVariant &value );
	void itemDataChanged( const QModelIndex &index, int role, const QVariant &value );
//private signals
	void itemDataAboutToBeChangedThread( const QModelIndex &index, int role, const QVariant &value, QPrivateSignal );
	void itemDataChangedThread( const QModelIndex &index, int role, const QVariant &value, QPrivateSignal );
	void rowsAboutToBeInsertedThread( const QModelIndex &parent, int first, int last, QPrivateSignal );
	void rowsInsertedThread( const QModelIndex &parent, int first, int last, QPrivateSignal );
	void rowsAboutToBeRemovedThread( const QModelIndex &parent, int first, int last, QPrivateSignal );
	void rowsRemovedThread( const QModelIndex &parent, int first, int last, QPrivateSignal );

private:
	class Impl;
	std::unique_ptr< Impl > impl_;
};

#endif // QT_LIST_MODEL_HPP

