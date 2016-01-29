#ifndef QT_PALETTE_H
#define QT_PALETTE_H

#include <QQuickItem>

class QtPalette : public QQuickItem
{
	Q_OBJECT

public:
	Q_PROPERTY( QColor MainWindowColor MEMBER mainWindowColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor HighlightColor MEMBER highlightColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor ToolTipColor MEMBER toolTipColor_ NOTIFY colorChanged )

	Q_PROPERTY( QColor TextColor MEMBER textColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor HighlightTextColor MEMBER highlightTextColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor TextBoxColor MEMBER textBoxColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor PlaceholderTextColor MEMBER placeholderTextColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor ToolTipTextColor MEMBER toolTipTextColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor BrightTextColor MEMBER brightTextColor_ NOTIFY colorChanged )

	Q_PROPERTY( QColor DarkHeaderColor MEMBER darkHeaderColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor LightPanelColor MEMBER lightPanelColor_ NOTIFY colorChanged )

	Q_PROPERTY( QColor MidDarkColor MEMBER midDarkColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor MidLightColor MEMBER midLightColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkColor MEMBER darkColor_ NOTIFY colorChanged )

	Q_PROPERTY( QColor NeutralTextColor MEMBER neutralTextColor_ NOTIFY colorChanged )
	Q_PROPERTY( QColor DisabledTextColor MEMBER disabledTextColor_ NOTIFY colorChanged )

	Q_PROPERTY( QColor LightShade MEMBER lightShade_ NOTIFY colorChanged )
	Q_PROPERTY( QColor LighterShade MEMBER lighterShade_ NOTIFY colorChanged )
	Q_PROPERTY( QColor LightestShade MEMBER lightestShade_ NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkShade MEMBER darkShade_ NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkerShade MEMBER darkerShade_ NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkestShade MEMBER darkestShade_ NOTIFY colorChanged )

    Q_PROPERTY( QColor OverlayDarkShade MEMBER overlayDarkShade_ NOTIFY colorChanged )
    Q_PROPERTY( QColor OverlayLightShade MEMBER overlayLightShade_ NOTIFY colorChanged )
    Q_PROPERTY( QColor OverlayDarkerShade MEMBER overlayDarkerShade_ NOTIFY colorChanged )
    Q_PROPERTY( QColor OverlayLighterShade MEMBER overlayLighterShade_ NOTIFY colorChanged )
    Q_PROPERTY( QColor OverlayTextColor MEMBER overlayTextColor_ NOTIFY colorChanged )

	Q_PROPERTY( QColor HighlightShade MEMBER highlightShade_ NOTIFY colorChanged )

	Q_PROPERTY( bool GlowStyle MEMBER glowStyle_ NOTIFY glowChanged )

	Q_PROPERTY( Theme theme READ theme WRITE setTheme NOTIFY themeChanged )

	Q_ENUMS( Theme )

	explicit QtPalette( QQuickItem* parent = nullptr );
	explicit QtPalette( QPalette& palette );

	QPalette toQPalette() const;

	enum Theme
	{
		Dark,
		Light,
		Wargaming,
		WorldOfTanks,
		WorldOfWarplanes,
		WorldOfWarships
	};

	Theme theme() const { return theme_; }
	void setTheme(Theme theme);

signals:
	void colorChanged();
	void glowChanged();
	void themeChanged(Theme theme);

private slots:
	void onPaletteChanged();
	void onColorChanged();

public slots:

private:
	QColor mainWindowColor_;
	QColor highlightColor_;
	QColor toolTipColor_;

	QColor textColor_;
	QColor highlightTextColor_;
	QColor textBoxColor_;
	QColor placeholderTextColor_;
	QColor toolTipTextColor_;
	QColor brightTextColor_;

	QColor darkHeaderColor_;
	QColor lightPanelColor_;

	QColor midDarkColor_;
	QColor midLightColor_;
	QColor darkColor_;

	QColor neutralTextColor_;
	QColor disabledTextColor_;

	QColor lightShade_;
	QColor lighterShade_;
	QColor lightestShade_;
	QColor darkShade_;
	QColor darkerShade_;
	QColor darkestShade_;

    QColor overlayDarkShade_;
    QColor overlayLightShade_;
	QColor overlayDarkerShade_;
    QColor overlayLighterShade_;
    QColor overlayTextColor_;

	QColor highlightShade_;

	bool	darkText_;
	bool	glowStyle_;
	int		timerid_;
	Theme	theme_;

protected:
	virtual void timerEvent(QTimerEvent* event) override;

};

#endif // QT_PALETTE_H
