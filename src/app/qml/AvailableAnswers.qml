import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0

GroupBox {
    id: availableAnswers

    title: i18n("Available Answers")
    Layout.preferredWidth: parent.width
    Layout.alignment: Qt.AlignHCenter
    Layout.fillHeight: true

    Flickable {
        anchors.fill: parent
        contentHeight: answerGrid.height
        clip: true

        Grid {
            id: answerGrid

            anchors.centerIn: parent
            spacing: 10

            columns: Math.max(1, parent.width / (((currentExercise != undefined && currentExercise["playMode"] != "rhythm") ? 120:119) + spacing))

            Component {
                id: answerOption

                Rectangle {
                    id: answerRectangle

                    property var model
                    property int index
                    property int position

                    width: (currentExercise != undefined && currentExercise["playMode"] != "rhythm") ? 120:119
                    height: (currentExercise != undefined && currentExercise["playMode"] != "rhythm") ? 40:59

                    Text {
                        id: option

                        property string originalText: model.name

                        visible: currentExercise != undefined && currentExercise["playMode"] != "rhythm"
                        text: i18nc("technical term, do you have a musician friend?", model.name)
                        width: parent.width - 4
                        anchors.centerIn: parent
                        horizontalAlignment: Qt.AlignHCenter
                        color: "black"
                        wrapMode: Text.Wrap
                    }
                    Image {
                        id: rhythmImage

                        anchors.centerIn: parent
                        visible: currentExercise != undefined && currentExercise["playMode"] == "rhythm"
                        source: (currentExercise != undefined && currentExercise["playMode"] == "rhythm") ? "exercise-images/" + model.name + ".png":""
                        fillMode: Image.Pad
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (exerciseView.state == "waitingForAnswer" && !animation.running) {
                                onExited()
                                internal.userAnswers.push({"name": option.originalText, "model": answerRectangle.model, "index": answerRectangle.index, "color": answerRectangle.color})
                                internal.currentAnswer++
                                if (internal.currentAnswer == currentExercise.numberOfSelectedOptions)
                                    checkAnswers()
                            }
                        }
                        hoverEnabled: Qt.platform.os != "android" && !animation.running
                        onEntered: {
                            answerRectangle.color = Qt.darker(answerRectangle.color, 1.1)
                            if (currentExercise["playMode"] != "rhythm" && exerciseView.state == "waitingForAnswer") {
                                if (parent.parent == answerGrid) {
                                    var array = [core.exerciseController.chosenRootNote()]
                                    model.sequence.split(' ').forEach(function(note) {
                                        array.push(core.exerciseController.chosenRootNote() + parseInt(note))
                                        pianoView.noteMark(0, core.exerciseController.chosenRootNote() + parseInt(note), 0, internal.colors[answerRectangle.index])
                                    })
                                    sheetMusicView.model = array
                                }
                            }
                            else {
                                var rightAnswers = core.exerciseController.selectedExerciseOptions
                                if (parent.parent == yourAnswersParent && internal.userAnswers[position].name != rightAnswers[position].name) {
                                    parent.border.color = "green"
                                    for (var i = 0; i < answerGrid.children.length; ++i) {
                                        if (answerGrid.children[i].model.name == rightAnswers[position].name) {
                                            parent.color = answerGrid.children[i].color
                                            break
                                        }
                                    }
                                    rhythmImage.source = "exercise-images/" + rightAnswers[position].name + ".png"
                                }
                            }
                        }
                        onExited: {
                            answerRectangle.color = internal.colors[answerRectangle.index]
                            if (currentExercise["playMode"] != "rhythm") {
                                if (parent.parent == answerGrid) {
                                    if (!animation.running)
                                        model.sequence.split(' ').forEach(function(note) {
                                            pianoView.noteUnmark(0, core.exerciseController.chosenRootNote() + parseInt(note), 0)
                                        })
                                    sheetMusicView.model = [core.exerciseController.chosenRootNote()]
                                }
                            }
                            else {
                                var rightAnswers = core.exerciseController.selectedExerciseOptions
                                if (parent.parent == yourAnswersParent && internal.userAnswers[position].name != rightAnswers[position].name) {
                                    parent.border.color = "red"
                                    parent.color = internal.userAnswers[position].color
                                    rhythmImage.source = "exercise-images/" + internal.userAnswers[position].name + ".png"
                                }
                            }
                        }
                    }
                }
            }
        }
        ScrollIndicator.vertical: ScrollIndicator { active: true }
    }
}