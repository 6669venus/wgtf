#ifndef QT_GLOBAL_SETTINGS_HPP
#define QT_GLOBAL_SETTINGS_HPP

#include <QQuickItem>

class QtGlobalSettings: public QQuickItem
{
	Q_OBJECT

public:
	QtGlobalSettings()
	{
		dragLocked_ = true;
	}

	Q_PROPERTY( bool dragLocked MEMBER dragLocked_ NOTIFY dragLockedChanged )

signals:
	void dragLockedChanged();

private:
	bool dragLocked_;
};

#endif
