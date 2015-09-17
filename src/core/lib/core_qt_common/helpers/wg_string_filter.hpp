#ifndef WG_STRING_FILTER_HPP
#define WG_STRING_FILTER_HPP

#include "../qt_new_handler.hpp"
#include <QObject>
#include <memory>

class QString;
class QVariant;

/**
 *	WGStringFilter
 *	A basic string-based filter wrapper around a StringFilter implementation.
 */
class WGStringFilter : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QVariant filter
				READ getFilter
				NOTIFY filterChanged )

	Q_PROPERTY( QString filterText
				READ getFilterText
				WRITE setFilterText
				NOTIFY filterTextChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGStringFilter();
	virtual ~WGStringFilter();

private:	
	QString getFilterText() const;
	void setFilterText( const QString & filterText );

	QVariant getFilter() const;

signals:
	void filterChanged();
	void filterTextChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // WG_STRING_FILTER_HPP
