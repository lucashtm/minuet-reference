import QtQuick 2.7

Score {
    property alias model: sequence.model
    property alias clef: clef
    property alias spaced: sequence.spaced
    
    function clearAllMarks() {
        clef.type = 0
        sequence.model = []
    }

    width: childrenRect.width; height: childrenRect.height
    FontLoader { id: bravura; source: "Bravura.otf" }
    antialiasing: true
    spacing: 10

//    clef: clef
    Clef { id: clef; type: 1 }

    Sequence { id: sequence }
}
