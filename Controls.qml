import QtQuick 2.0
import QtQuick.Controls 2.2

import "util.js" as Util

Rectangle {
    id: root
    width: parent.width
    color: Qt.rgba(0.6, 0.3, 0.3, 0.6)

    property int elapsed: 0
    property int duration: 0
    property bool playing: false

    signal seeked(real value)
    signal play
    signal pause
    signal next
    signal prev

    Rectangle {
        id: sliderArea

        width: parent.width
        height: 20
        anchors.top: parent.top
        anchors.topMargin: 10
        z: 2

        color: "transparent"

        Rectangle {
            id: elapsedText
            width: 50
            height: parent.height
            color: "transparent"

            property alias text: textComponent.text

            anchors {
                left: parent.left
                top: parent.top
            }

            Text {
                id: textComponent
                anchors.centerIn: parent

                text: Util.convertToDurationString(0)
            }
        }

        Rectangle {
            id: durationText
            width: 50
            height: parent.height
            color: "transparent"

            anchors {
                right: parent.right
                top: parent.top
            }

            Text {
                text: Util.convertToDurationString(duration)
                anchors.centerIn: parent
            }
        }
        Slider {
            id: slider
            property bool isPressed: false
            height: parent.height
            width: 300
            anchors {
                left: elapsedText.right
                leftMargin: 5
                right: durationText.left
                rightMargin: 5
            }
            onPressedChanged: {
                isPressed = pressed
                if (!pressed)
                    root.seeked(value)
            }
            onValueChanged: if (isPressed) elapsedText.text = Util.convertToDurationString(value * duration)
        }
    }

    Rectangle {
        id: buttonArea
        width: parent.width
        anchors {
            top: sliderArea.bottom
            bottom: parent.bottom
        }
        z: 2
        color: "transparent"

        Button {
            id: btnPlayPause
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 50
            height: 50
            contentItem: Text {
                text: root.playing ? "||" : "▶"
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: btnPlayPause.down ? "#d6d6d6" : "#f6f6f6"
                radius: 25
            }

            onClicked: root.playing ? root.pause() : root.play()
        }

        Button {
            id: btnPrev
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: -60
            anchors.verticalCenter: parent.verticalCenter
            width: 40
            height: 40
            onClicked: root.prev()

            contentItem: Text {
                text: "◀◀"
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: btnPrev.down ? "#d6d6d6" : "#f6f6f6"
                radius: 20
            }
        }

        Button {
            id: btnNext
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 60
            anchors.verticalCenter: parent.verticalCenter
            width: 40
            height: 40
            onClicked: root.next()
            contentItem: Text {
                text: "▶▶"
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: btnNext.down ? "#d6d6d6" : "#f6f6f6"
                radius: 20
            }
        }
    }

    MouseArea {
        // trick to block mouseinput to be forward to the list behind
        anchors.fill: parent
        z: 1
    }

    onElapsedChanged: updateSlider()
    onDurationChanged: updateSlider()

    function updateSlider(){
        if(!slider.isPressed) {
            slider.value = elapsed / duration
            elapsedText.text = Util.convertToDurationString(elapsed)
        }
    }
}
