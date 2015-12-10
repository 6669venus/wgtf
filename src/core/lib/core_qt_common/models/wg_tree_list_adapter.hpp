#ifndef WG_TREE_LIST_ADAPTER_HPP
#define WG_TREE_LIST_ADAPTER_HPP

#include <QAbstractItemModel>
#include <QQmlListProperty>

#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/models/adapters/i_list_adapter.hpp"
#include "qt_model_macros.hpp"
#include <memory>

class IModelExtension;

/**
 * This is the Qt data model which accepts a QModelIndex and adapts it into a list-friendly format
 * so that it may use its own extensions and allow the use of a single ITreeModel to represent
 * directory structures.
 */
class WGTreeListAdapter : public IListAdapter
{
	Q_OBJECT

	Q_PROPERTY( QVariant	sourceIndex
				READ		getSourceIndex
				WRITE		setSourceIndex
				NOTIFY		sourceIndexChanged )

	Q_PROPERTY( QQmlListProperty< IModelExtension > extensions 
				READ getExtensions )

	Q_CLASSINFO( "DefaultProperty", "extensions" )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGTreeListAdapter();
	virtual ~WGTreeListAdapter();
	
	QAbstractItemModel * model() const Q_DECL_OVERRIDE;
			
	QModelIndex adaptedIndex(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	template< typename T >
	void registerExtension()
	{
		auto extension = new T();
		extension->setParent( this );
		registerExtension( extension );
	}

	QVariant getSourceIndex() const;
	void setSourceIndex( const QVariant & sourceIndex );
	
private:
	void registerExtension( IModelExtension * extension );

	// QAbstractItemModel Start
	QHash< int, QByteArray > roleNames() const Q_DECL_OVERRIDE;
	int rowCount( const QModelIndex &parent ) const Q_DECL_OVERRIDE;
	// QAbstractItemModel End

	QQmlListProperty< IModelExtension > getExtensions() const;

	static void appendExtension( QQmlListProperty< IModelExtension > * property, IModelExtension * value );
	static IModelExtension * extensionAt( QQmlListProperty< IModelExtension > * property, int index );
	static void clearExtensions( QQmlListProperty< IModelExtension > * property );
	static int countExtensions( QQmlListProperty< IModelExtension > * property );

signals:
	void sourceIndexChanged();

private:
	class Impl;
	std::unique_ptr< Impl > impl_;
};

#endif // WG_TREE_LIST_ADAPTER_HPP