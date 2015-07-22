import QtQuick 2.0
import QtQuick.Controls 1.2
import WGControls 1.0

WGDropDownBox{
    id: filter

    model: ListModel {
        id: model
        ListElement { text: "" }
        ListElement { text: "dsAnimation" }
        ListElement { text: "dsAnimProj" }
        ListElement { text: "animGroup" }
        ListElement { text: "dsBrushMaterial" }
        ListElement { text: "dsCinema" }
        ListElement { text: "dsCurve" }
        ListElement { text: "dsFacade" }
        ListElement { text: "dsFont" }
        ListElement { text: "dsMaterial" }
        ListElement { text: "dsMenuFlow" }
        ListElement { text: "dsModel" }
        ListElement { text: "dsMovie" }
        ListElement { text: "dsObject" }
        ListElement { text: "dsParticle" }
        ListElement { text: "dsPlayerProfile" }
        ListElement { text: "dsPrefab" }
        ListElement { text: "dsSoundCue" }
        ListElement { text: "dsSubstance" }
        ListElement { text: "dsTree" }
        ListElement { text: "dsWorkspace" }
        ListElement { text: "dsWorld" }
        ListElement { text: "png" }
        ListElement { text: "tif" }
        ListElement { text: "lua" }
    }
}
