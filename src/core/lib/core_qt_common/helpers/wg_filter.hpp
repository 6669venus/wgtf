#ifndef WG_FILTER_HPP
#define WG_FILTER_HPP

#include "../qt_new_handler.hpp"
#include <QObject>
#include <QtQml>
#include <memory>

class IItemFilter;

/**
 *	WGFilter
 *	The parent of all filter components. Grants access to the filter.
 */
class WGFilter : public QObject
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	WGFilter() {};
	virtual ~WGFilter() {};
	
	virtual IItemFilter * getFilter() const { return nullptr; }
};

QML_DECLARE_TYPE( WGFilter )

#endif // WG_FILTER_HPP
