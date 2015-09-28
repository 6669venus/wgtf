#ifndef WG_TREE_MODEL_HPP
#define WG_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QQmlListProperty>

#include "core_data_model/i_tree_model.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "qt_model_macros.hpp"


class IModelExtension;


/**
 *	This is the Qt data model which Qt views will use to display data.
 */
class WGTreeModel : public QAbstractItemModel
{
	Q_OBJECT

	Q_PROPERTY( QVariant source 
				READ getSource 
				WRITE setSource 
				NOTIFY sourceChanged)
	Q_PROPERTY( QQmlListProperty< IModelExtension > extensions 
				READ getExtensions )
	Q_CLASSINFO( "DefaultProperty", "extensions" )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGTreeModel();
	virtual ~WGTreeModel();

	void source( ITreeModel * source );
	ITreeModel * source() const;

	Q_INVOKABLE QModelIndex index(
		int row, int column, const QModelIndex & parent = QModelIndex() ) const Q_DECL_OVERRIDE;
	Q_INVOKABLE QModelIndex parent( const QModelIndex &child ) const Q_DECL_OVERRIDE;

	template< typename T >
	void registerExtension()
	{
		auto extension = new T();
		extension->setParent( this );
		registerExtension( extension );
	}

protected:

	// Used to retrieve the underlying data model. In WGTreeModel this will be the source, but it could be
	// a filtered or altered 
	virtual ITreeModel* getModel() const;
    virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

private:
	void registerExtension( IModelExtension * extension );

	// QAbstractItemModel Start
	QHash< int, QByteArray > roleNames() const Q_DECL_OVERRIDE;

	int columnCount( const QModelIndex &parent ) const Q_DECL_OVERRIDE;
	bool hasChildren( const QModelIndex &parent ) const Q_DECL_OVERRIDE;

	QVariant data( const QModelIndex &index, int role ) const Q_DECL_OVERRIDE;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) Q_DECL_OVERRIDE;
	//QAbstractItemModel End

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
	EVENT_DECL( ITreeModel )
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

#endif // QT_TREE_MODEL_HPP

