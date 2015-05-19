import QtQuick 2.0
import QtQuick.Layouts 1.0

//WGlabel that wraps multiple lines.

WGLabel {
    Layout.minimumHeight: lineCount * panelProps.rowHeight_
    formLabel_: false
    wrapMode: Text.WordWrap
}
