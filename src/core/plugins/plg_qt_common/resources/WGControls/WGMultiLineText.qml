import QtQuick 2.3
import QtQuick.Layouts 1.1

//WGlabel that wraps multiple lines.

WGLabel {
    Layout.minimumHeight: lineCount * panelProps.rowHeight_
    formLabel_: false
    wrapMode: Text.WordWrap
}
