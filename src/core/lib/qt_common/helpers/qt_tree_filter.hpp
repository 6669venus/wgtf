#ifndef QT_TREE_PANEL_FILTER_HPP
#define QT_TREE_PANEL_FILTER_HPP

#include "qt_common/qt_new_handler.hpp"

#include <QObject>
#include <QString>

#include <memory>

class QtTreeFilter: public QObject
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
	QtTreeFilter();
	virtual ~QtTreeFilter();

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
	std::unique_ptr<Implementation> impl_;
};

#endif
