#ifndef QT_GLOBAL_SETTINGS_HPP
#define QT_GLOBAL_SETTINGS_HPP

#include <QtQuick/QQuickItem>

class QtGlobalSettings: public QQuickItem
{
	Q_OBJECT

public:
	QtGlobalSettings()
	{
		dragLocked_ = true;
		wgCopyableEnabled_ = false;
	}

	Q_PROPERTY( bool dragLocked MEMBER dragLocked_ NOTIFY dragLockedChanged )
	Q_PROPERTY( bool wgCopyableEnabled MEMBER wgCopyableEnabled_ NOTIFY wgCopyableEnabledChanged )

signals:
	void dragLockedChanged();
	void wgCopyableEnabledChanged();

private:
	bool dragLocked_;
	bool wgCopyableEnabled_;
};

#endif
