#ifndef ASSET_BROWSER_LIST_FILTER_HPP
#define ASSET_BROWSER_LIST_FILTER_HPP

/// List filter for Asset Browser

#include "qt_common/qt_new_handler.hpp"

#include <QObject>
#include <QString>
#include <QVariant>

#include <memory>


class AssetBrowserListFilter
	: public QObject
{
	Q_OBJECT

	Q_PROPERTY( QVariant source 
				READ getSource 
				WRITE setSource 
				NOTIFY sourceChanged )
	Q_PROPERTY( QString filter
				READ getFilter
				WRITE setFilter
				NOTIFY filterChanged )
	Q_PROPERTY( QVariant filteredSource
				READ getFilteredSource
				NOTIFY sourceChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	AssetBrowserListFilter();
	virtual ~AssetBrowserListFilter();

private:
	QVariant getSource() const;
	QString getFilter() const;
	QVariant getFilteredSource() const;

	void setSource( const QVariant & source );
	void setFilter( const QString & filter );

signals:
	void sourceChanged();
	void filterChanged();

private:
	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};

#endif // ASSET_BROWSER_LIST_FILTER_HPP
