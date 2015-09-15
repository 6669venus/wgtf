#ifndef WG_FILTERED_LIST_MODEL_HPP
#define WG_FILTERED_LIST_MODEL_HPP

#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include <memory>

class WGFilteredListModel: public WGListModel
{
	Q_OBJECT

	Q_PROPERTY( QVariant filter
				READ getFilter
				WRITE setFilter
				NOTIFY filterChanged )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGFilteredListModel();
	virtual ~WGFilteredListModel();

private:
	QVariant getFilter() const;
	void setFilter( const QVariant & filter );

signals:
	void filterChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

#endif // WG_FILTERED_LIST_MODEL_HPP
