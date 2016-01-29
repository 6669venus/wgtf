#ifndef WG_FILTER_HPP
#define WG_FILTER_HPP

#include "../qt_new_handler.hpp"
#include "core_data_model/i_item_role.hpp"
#include "wg_types/hash_utilities.hpp"
#include <QObject>
#include <QtQml>
#include <QString>
#include <memory>

class IItemFilter;

/**
 *	WGFilter
 *	The parent of all filter components. Grants access to the filter.
 */
class WGFilter : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QString itemRole
				READ getItemRole
				WRITE setItemRole
				NOTIFY itemRoleChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGFilter() : itemRole_( std::make_pair( "", 0 ) ) {};
	virtual ~WGFilter() {};
	
	virtual IItemFilter * getFilter() const { return nullptr; }
	
	QString getItemRole() const { return QString::fromStdString( itemRole_.first ); }
	void setItemRole( const QString & itemRole ) 
	{ 
		std::string roleName = itemRole.toUtf8().constData(); 
		unsigned int roleId = static_cast<unsigned int>(HashUtilities::compute( roleName.c_str() ));
		itemRole_ = std::make_pair( roleName.c_str(), roleId );
		updateInternalItemRole();
		emit itemRoleChanged();
	}

protected:
	virtual void updateInternalItemRole() {}

signals:
	void itemRoleChanged();

protected:
	ItemRole itemRole_;
};

QML_DECLARE_TYPE( WGFilter )

#endif // WG_FILTER_HPP
