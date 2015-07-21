#include "qt_palette.hpp"
#include <QPalette>
#include <QColor>

QtPalette::QtPalette( QQuickItem* parent )
	: QQuickItem( parent )
{
	mainWindowColor_ = QColor( 56, 58, 61 );
	highlightColor_ = QColor( 51, 153, 255 );
	highlightTextColor_ = QColor( 255, 255, 255);

	toolTipColor_ = highlightColor_;
	toolTipColor_.setAlpha( 220 );

	textColor_ = QColor( 255, 255, 255 );

	darkText_ = false;
	glowStyle_ = false;

	toolTipTextColor_ = QColor( 255, 255, 255, 220 );
	brightTextColor_ = QColor( 255, 0, 0 );

	lightShade_ = QColor( 255, 255, 255, 16 );
	lighterShade_ = QColor( 255, 255, 255, 32 );
	lightestShade_ = QColor( 255, 255, 255, 48 );
	darkShade_ = QColor( 0, 0, 0, 32 );
	darkerShade_ = QColor( 0, 0, 0, 64 );
	darkestShade_ = QColor( 0, 0, 0, 96 );

	highlightShade_ = highlightColor_;
	highlightShade_.setAlpha( 128 );

	darkHeaderColor_ = mainWindowColor_.darker( 125 );
	lightPanelColor_ = mainWindowColor_.lighter( 132 );

	midDarkColor_ = mainWindowColor_.darker( 110 );
	midLightColor_ = mainWindowColor_.lighter( 120 );
	darkColor_ = mainWindowColor_.darker( 150 );

	if (darkText_)
	{
		neutralTextColor_ = textColor_.lighter( 130 );
		disabledTextColor_ = darkestShade_;
		textBoxColor_ = lightestShade_;
		placeholderTextColor_ = darkerShade_;
	}
	else
	{
		neutralTextColor_ = textColor_.darker( 130 );
		disabledTextColor_ = lightestShade_;
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lighterShade_;
	}
}


QtPalette::QtPalette( QPalette& palette )
{
	mainWindowColor_ = palette.color( QPalette::Window );
	highlightColor_ = palette.color( QPalette::Link );
	highlightTextColor_ = palette.color( QPalette::HighlightedText );

	toolTipColor_ = palette.color( QPalette::ToolTipBase );
	toolTipColor_.setAlpha( 220 );

	textColor_ = palette.color( QPalette::Text );

	glowStyle_ = false;

	toolTipTextColor_ = palette.color( QPalette::ToolTipText );
	brightTextColor_ = palette.color( QPalette::BrightText );

	lightShade_ = QColor( 255, 255, 255, 16 );
	lighterShade_ = QColor( 255, 255, 255, 32 );
	lightestShade_ = QColor( 255, 255, 255, 48 );
	darkShade_ = QColor( 0, 0, 0, 32 );
	darkerShade_ = QColor( 0, 0, 0, 64 );
	darkestShade_ = QColor( 0, 0, 0, 96 );

	highlightShade_ = highlightColor_;
	highlightShade_.setAlpha( 128 );

	darkHeaderColor_ = palette.color( QPalette::Mid );
	lightPanelColor_ = palette.color( QPalette::Light );

	midDarkColor_ = palette.color( QPalette::Base );
	midLightColor_ = palette.color( QPalette::Midlight );
	darkColor_ = palette.color( QPalette::Dark );

	neutralTextColor_ = palette.color( QPalette::WindowText );
	disabledTextColor_ = palette.color( QPalette::Disabled, QPalette::WindowText );

	darkText_ = disabledTextColor_ == darkestShade_;

	if (darkText_)
	{
		textBoxColor_ = lightestShade_;
		placeholderTextColor_ = darkerShade_;
	}
	else
	{
		textBoxColor_ = darkerShade_;
		placeholderTextColor_ = lighterShade_;
	}
}


QPalette QtPalette::toQPalette() const
{
	QPalette palette;

	palette.setColor( QPalette::Window, mainWindowColor_ );
	palette.setColor( QPalette::WindowText, neutralTextColor_ );
	palette.setColor( QPalette::Base, midDarkColor_ );
	palette.setColor( QPalette::AlternateBase, mainWindowColor_ );
	palette.setColor( QPalette::ToolTipBase, toolTipColor_ );
	palette.setColor( QPalette::Text, textColor_ );
	palette.setColor( QPalette::ToolTipText, toolTipTextColor_ );
	palette.setColor( QPalette::Button, mainWindowColor_ );
	palette.setColor( QPalette::ButtonText, neutralTextColor_ );
	palette.setColor( QPalette::BrightText, brightTextColor_ );
	palette.setColor( QPalette::Link, highlightColor_ );
	palette.setColor( QPalette::Light, lightPanelColor_ );
	palette.setColor( QPalette::Midlight, midLightColor_ );
	palette.setColor( QPalette::Mid, darkHeaderColor_ );
	palette.setColor( QPalette::Dark, darkColor_ );
	palette.setColor( QPalette::Highlight, highlightColor_ );
	palette.setColor( QPalette::HighlightedText, highlightTextColor_ );
	palette.setColor( QPalette::Disabled,QPalette::Window, mainWindowColor_ );
	palette.setColor( QPalette::Disabled,QPalette::WindowText, disabledTextColor_ );
	palette.setColor( QPalette::Disabled,QPalette::Base, midDarkColor_ );
	palette.setColor( QPalette::Disabled,QPalette::AlternateBase, mainWindowColor_ );
	palette.setColor( QPalette::Disabled,QPalette::ToolTipBase, toolTipColor_ );
	palette.setColor( QPalette::Disabled,QPalette::Text, disabledTextColor_ );
	palette.setColor( QPalette::Disabled,QPalette::Button, mainWindowColor_ );
	palette.setColor( QPalette::Disabled,QPalette::ButtonText, disabledTextColor_ );
	palette.setColor( QPalette::Disabled,QPalette::BrightText, brightTextColor_ );
	palette.setColor( QPalette::Disabled,QPalette::Link, disabledTextColor_ );
	palette.setColor( QPalette::Disabled,QPalette::Highlight, mainWindowColor_ );
	palette.setColor( QPalette::Disabled,QPalette::HighlightedText, disabledTextColor_ );

	return palette;
}
