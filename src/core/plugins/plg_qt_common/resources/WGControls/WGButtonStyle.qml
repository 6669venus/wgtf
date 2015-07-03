import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

ButtonStyle {

    //invisible label as handled in button control.
    label: Text{
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: false
    }

    background: WGButtonFrame{
		id: buttonFrame

        visible: {
			if (control.noInteraction_){
				false
			} else if (control.noFrame_ && !control.hovered && !control.pressed && !control.checked && !control.activeFocus) {
                false
            } else {
                true
            }
        }

        color: {
			if(control.checked && !control.noInteraction_){
                palette.HighlightColor
            } else {
                palette.LightShade
            }
        }

        borderColor_: {
            if (control.enabled && control.checked){
                palette.HighlightColor
            } else if (control.enabled && !control.checked){
                palette.DarkerShade
            } else if (!control.enabled){
                palette.DarkShade
            }
        }

        innerBorderColor_: {
            if (control.enabled && control.pressed){
                palette.DarkerShade
            } else if (control.enabled && !control.pressed && control.activeFocus && control.checked){
                palette.DarkShade
            } else if (control.enabled && !control.pressed && control.activeFocus && !control.checked){
                palette.HighlightShade
            } else if (control.enabled && !control.pressed && !control.activeFocus){
                palette.LighterShade
            } else if (!control.enabled){
                "transparent"
            }
        }

        highlightColor_: {
			if (control.pressed && !control.noInteraction_){
                palette.DarkerShade
			} else if (control.hovered && !control.pressed && !palette.GlowStyle && !control.noInteraction_){
                palette.LighterShade
			} else if (control.hovered && control.checked && !control.pressed && !control.noInteraction_){
                palette.LightestShade
            } else {
                "transparent"
            }
		}

		// Not sure if states are better for this or just as bad.
		// Leaving here for now because I don't want to type this out again

		/*
		visible: true
		color: palette.LightShade
		borderColor_: palette.DarkerShade
		innerBorderColor_: palette.LighterShade
		highlightColor_: "transparent"

		states: [
			State {
				name: "DISABLED"
				when: !control.enabled
				PropertyChanges {
					target: buttonFrame;
					borderColor_: palette.DarkShade
					innerBorderColor_: "transparent"
				}
			},
			State {
				name: "PRESSED"
				when: control.pressed && !control.noInteraction_
				PropertyChanges {
					target: buttonFrame;
					innerBorderColor_: palette.DarkerShade
					highlightColor_: palette.DarkerShade
				}
			},
			State {
				name: "HOVERED"
				when: control.hovered && !control.pressed && !control.checked && !control.noInteraction_
				PropertyChanges {
					target: buttonFrame;
					highlightColor_: palette.LighterShade
				}
			},
			State {
				name: "HOVERED CHECKED"
				when: control.hovered && !control.pressed && control.checked && !control.noInteraction_
				PropertyChanges {
					target: buttonFrame;
					color: palette.HighlightColor
					borderColor_: palette.HighlightColor
					innerBorderColor_: palette.DarkShade
					highlightColor_: palette.LightestShade
				}
			},
			State {
				name: "CHECKED"
				when: control.checked && !control.hovered && !control.noInteraction_
				PropertyChanges {
					target: buttonFrame;
					color: palette.HighlightColor
					borderColor_: palette.HighlightColor
					innerBorderColor_: palette.DarkShade
				}
			},
			State {
				name: "FOCUS"
				when: control.activeFocus && !control.checked && !control.noInteraction_
				PropertyChanges {
					target: buttonFrame;
					innerBorderColor_: palette.HighlightShade
				}
			},
			State {
				name: "FOCUS CHECKED"
				when: control.activeFocus && control.checked && !control.noInteraction_
				PropertyChanges {
					target: buttonFrame;
					innerBorderColor_: palette.DarkShade
				}
			},
			State {
				name: "HIDDEN"
				when: control.noInteraction_ || (control.noFrame_ && !control.hovered && !control.pressed && !control.checked && !control.activeFocus)
				PropertyChanges {
					target: buttonFrame;
					visible: false
				}
			}

		]*/

        hovered_: control.hovered

        radius_: control.radius_
    }
}
