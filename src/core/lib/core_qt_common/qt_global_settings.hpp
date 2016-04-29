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
		wgNativeRendering_ = false;
	}

	Q_PROPERTY( bool dragLocked MEMBER dragLocked_ NOTIFY dragLockedChanged )
	Q_PROPERTY( bool wgCopyableEnabled MEMBER wgCopyableEnabled_ NOTIFY wgCopyableEnabledChanged )
	Q_PROPERTY(bool wgNativeRendering MEMBER wgNativeRendering_ CONSTANT )

    void filePreferenceChangeEvent()
    {
        emit preferencesChanged();
    }

signals:
	void dragLockedChanged();
	void wgCopyableEnabledChanged();
    void preferencesChanged();

private:
	bool dragLocked_;
	bool wgCopyableEnabled_;
	bool wgNativeRendering_;
};

#endif
