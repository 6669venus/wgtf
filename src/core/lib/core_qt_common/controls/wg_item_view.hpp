#ifndef WG_ITEM_VIEW_HPP
#define WG_ITEM_VIEW_HPP

#include <memory>
#include <QQuickItem>
#include <QQmlListProperty>

class IModelExtension;
class QAbstractItemModel;
class QString;

class WGItemView : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY( QAbstractItemModel * model READ getModel WRITE setModel NOTIFY modelChanged )

	Q_PROPERTY( QStringList roles READ getRoles WRITE setRoles )
	Q_PROPERTY( QQmlListProperty< IModelExtension > extensions READ getExtensions )

	Q_PROPERTY( QAbstractItemModel * extendedModel READ getExtendedModel NOTIFY extendedModelChanged )
	//Enable for headers once body works.
	//Q_PROPERTY( QObject * headerData READ getHeaderData NOTIFY headerDataChanged )

public:
	WGItemView();
	~WGItemView();

signals:
	void modelChanged();
	void extendedModelChanged();
	void headerDataChanged();

private:
	QAbstractItemModel * getModel() const;
	void setModel( QAbstractItemModel * model );

	QStringList getRoles() const;
	void setRoles( const QStringList & roles );

	QQmlListProperty< IModelExtension > getExtensions() const;
	static void appendExtension( QQmlListProperty< IModelExtension > * property, IModelExtension * value );
	static int countExtensions( QQmlListProperty< IModelExtension > * property );
	static IModelExtension * extensionAt( QQmlListProperty< IModelExtension > * property, int index );
	static void clearExtensions( QQmlListProperty< IModelExtension > * property );

	QAbstractItemModel * getExtendedModel() const;
	QObject * getHeaderData() const;

	void refresh();

	struct Impl;
	std::unique_ptr< Impl > impl_;
};

#endif//WG_ITEM_VIEW_HPP