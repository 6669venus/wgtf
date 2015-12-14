#ifndef WG_ACTION_HPP
#define WG_ACTION_HPP

#include "../qt_new_handler.hpp"

#include <QQuickItem>

#include <memory>

class WGAction : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY( bool active READ getActive WRITE setActive )
	Q_PROPERTY( QString actionId READ getActionId WRITE setActionId )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGAction( QQuickItem * parent = nullptr );
	virtual ~WGAction();

protected:
	void componentComplete();

	bool getActive() const;
	void setActive( bool active );

	QString getActionId() const;
	void setActionId( const QString& actionId );

signals:
	void triggered();

private:
	struct Implementation;
	std::unique_ptr< Implementation > impl_;
};

#endif //WG_ACTION_HPP