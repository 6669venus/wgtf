#ifndef QT_PALETTE_H
#define QT_PALETTE_H

#include <QQuickItem>

class QtPalette : public QQuickItem
{
	Q_OBJECT

public:
	Q_PROPERTY( QColor mainWindowColor MEMBER mainWindowColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor highlightColor MEMBER highlightColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor toolTipColor MEMBER toolTipColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor textColor MEMBER textColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor highlightTextColor MEMBER highlightTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor textBoxColor MEMBER textBoxColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor placeholderTextColor MEMBER placeholderTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor toolTipTextColor MEMBER toolTipTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor brightTextColor MEMBER brightTextColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor darkHeaderColor MEMBER darkHeaderColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor lightPanelColor MEMBER lightPanelColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor midDarkColor MEMBER midDarkColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor midLightColor MEMBER midLightColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor darkColor MEMBER darkColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor neutralTextColor MEMBER neutralTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor disabledTextColor MEMBER disabledTextColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor lightShade MEMBER lightShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor lighterShade MEMBER lighterShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor lightestShade MEMBER lightestShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor darkShade MEMBER darkShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor darkerShade MEMBER darkerShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor darkestShade MEMBER darkestShade_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor overlayDarkShade MEMBER overlayDarkShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor overlayLightShade MEMBER overlayLightShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor overlayDarkerShade MEMBER overlayDarkerShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor overlayLighterShade MEMBER overlayLighterShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor overlayTextColor MEMBER overlayTextColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor highlightShade MEMBER highlightShade_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( bool glowStyle MEMBER glowStyle_ NOTIFY glowChanged )

	// \/\/\/\/ DEPRECATED \/\/\/\/

	Q_PROPERTY( QColor MainWindowColor MEMBER mainWindowColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor HighlightColor MEMBER highlightColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor ToolTipColor MEMBER toolTipColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor TextColor MEMBER textColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor HighlightTextColor MEMBER highlightTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor TextBoxColor MEMBER textBoxColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor PlaceholderTextColor MEMBER placeholderTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor ToolTipTextColor MEMBER toolTipTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor BrightTextColor MEMBER brightTextColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor DarkHeaderColor MEMBER darkHeaderColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor LightPanelColor MEMBER lightPanelColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor MidDarkColor MEMBER midDarkColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor MidLightColor MEMBER midLightColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkColor MEMBER darkColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor NeutralTextColor MEMBER neutralTextColor_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor DisabledTextColor MEMBER disabledTextColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor LightShade MEMBER lightShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor LighterShade MEMBER lighterShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor LightestShade MEMBER lightestShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkShade MEMBER darkShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkerShade MEMBER darkerShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor DarkestShade MEMBER darkestShade_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor OverlayDarkShade MEMBER overlayDarkShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor OverlayLightShade MEMBER overlayLightShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor OverlayDarkerShade MEMBER overlayDarkerShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor OverlayLighterShade MEMBER overlayLighterShade_ WRITE dummySet NOTIFY colorChanged )
	Q_PROPERTY( QColor OverlayTextColor MEMBER overlayTextColor_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( QColor HighlightShade MEMBER highlightShade_ WRITE dummySet NOTIFY colorChanged )

	Q_PROPERTY( bool GlowStyle MEMBER glowStyle_ NOTIFY glowChanged )

	// /\/\/\/\ DEPRECATED /\/\/\/\

	Q_PROPERTY( Theme theme READ theme WRITE setTheme NOTIFY themeChanged )

	Q_ENUMS( Theme )

	// Fake set function to make colors read-only (for now)
	void dummySet(QColor) {};

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
