#ifndef WG_TOKENIZED_STRING_FILTER_HPP
#define WG_TOKENIZED_STRING_FILTER_HPP

#include "../qt_new_handler.hpp"
#include <QObject>
#include <memory>

class QString;
class QVariant;

/**
 *	WGTokenizedStringFilter
 *	A tokenized string filter wrapper around a TokenizedStringFilter implementation.
 */
class WGTokenizedStringFilter : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QVariant filter
				READ getFilter
				NOTIFY filterChanged )

	Q_PROPERTY( QString filterText
				READ getFilterText
				WRITE setFilterText
				NOTIFY filterTextChanged )

	Q_PROPERTY( QString splitterChar
				READ getSplitterChar
				WRITE setSplitterChar
				NOTIFY splitterCharChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGTokenizedStringFilter();
	virtual ~WGTokenizedStringFilter();

private:	
	QString getFilterText() const;
	void setFilterText( const QString & filterText );
	QVariant getFilter() const;

	QString getSplitterChar() const;
	void setSplitterChar( const QString & splitter );

signals:
	void filterChanged();
	void filterTextChanged();
	void splitterCharChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // WG_TOKENIZED_STRING_FILTER_HPP
