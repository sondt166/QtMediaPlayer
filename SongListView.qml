import QtQuick 2.0
import QtQuick.Controls 2.2
import MediaPlayer 1.0

import "util.js" as Util

ScrollView {
    id: root

    signal chooseSong(int index)
    signal removeSong(int index, string title)

    signal collapseOthers(int currentIndex)
    onCollapseOthers: currentFocusIndex = currentIndex

    property int currentFocusIndex: -1
    property int currentPlayIndex: -1
    property bool playing: false
    property int currentIndex: 0
    property alias listView: listComponent

    function decreaseIndex() {
        currentIndex--;
        if (currentIndex < 0)
            currentIndex = 0
        listComponent.currentIndex = currentIndex - 1;
    }

    function increseIndex() {
        currentIndex++;
        if (currentIndex >= listComponent.count)
            currentIndex = listComponent.count - 1
        listComponent.currentIndex = currentIndex + 1;
    }

    ListView {
        id: listComponent
        anchors.fill: parent
        clip: true
        smooth: true
//        snapMode: ListView.SnapToItem
        // Increase Flick speed
//        cacheBuffer: 4

        model: SongModel {
            list: songList
        }

        displaced: Transition {
            NumberAnimation { property: "y"; duration: 200 }
        }

        delegate: Item {
            id: item
            width: parent.width
            height: 70

            property bool hovered: false

            Rectangle {
                id: mask
                width: rect.width
                height: rect.height
                color: "transparent"
                z: 2
            }

            Rectangle {
                id: hoverMask
                width: rect.width
                height: rect.height
                state: "unhovered"
                color: "transparent"
                z: 3

                readonly property color hoveredColor:Qt.rgba(1, 0.5, 0.5, 0.2)
                readonly property color unhoveredColor: Qt.rgba(1, 1, 1, 0)

                states: [
                    State {
                        name: "hovered"
                        when: item.hovered

                        PropertyChanges {
                            target: hoverMask
                            color: hoveredColor
                        }
                    },
                    State {
                        name: "unhovered"
                        when: !item.hovered

                        PropertyChanges {
                            target: hoverMask
                            color: unhoveredColor
                        }
                    }
                ]

                Behavior on color {
                    ColorAnimation {
                        duration: item.hovered ? 500 : 0
                    }
                }
            }

            Rectangle {
                id: rect
                width: parent.width
                height: parent.height - 1
                z: 1

                Rectangle {
                    id: indexRect
                    width: 50
                    height: parent.height
                    color: root.currentPlayIndex == index ? "darkred" : "black"

                    Text {
                        anchors.centerIn: parent
                        text: root.currentPlayIndex == index ? (playing ? "▶" : "⏸") : index + 1
                        font {
                            family: "Tahoma"
                            pixelSize: 16
                        }
                        color: "white"
                    }


                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                        }
                    }
                }

                Rectangle {
                    id: titleRect
                    anchors.left: indexRect.right
                    width: parent.width - duration.width - indexRect.width - thumbnailRect.width
                    height: parent.height
                    color: (index & 1 ) ? Qt.darker(Qt.rgba(0.2, 0.2, 0.2, 1)) :Qt.rgba(0.2, 0.2, 0.2, 1)

                    Text {
                        id: title
                        text: model.Name
                        elide: Qt.ElideRight

                        anchors.top: parent.top
                        anchors.topMargin: 10
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 20

                        font {
                            family: "Tahoma"
                            pixelSize: 16
                            bold: true
                        }
                        color: "white"
                    }

                    Text {
                        id: artist
                        text: model.Artist
                        elide: Qt.ElideRight

                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 20

                        font {
                            family: "Tahoma"
                            pixelSize: 12
                            italic: true
                        }
                        color: "gray"
                    }
                }

                Rectangle {
                    id: thumbnailRect
                    anchors.left: titleRect.right
                    height: parent.height
                    width: height

                    ImageItem {
                        id: thumbnail
                        anchors.fill: parent
                        image: model.Thumbnail
                    }
                }

                Rectangle {
                    id: duration
                    color: "lightgray"
                    width: 60
                    height: parent.height
                    anchors.right: parent.right

                    Text {
                        text: Util.convertToDurationString(model.Duration)
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                Button {
                    id: btnRemove
                    width: 100
                    height: parent.height
                    anchors.left: duration.right
                    palette {
                        button: btnRemove.enabled ? "red" : "gray"
                        buttonText: "white"
                    }
                    contentItem:  Text {
                        text: "Remove"
                        font.pixelSize: 15
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }

                    onClicked: root.removeSong(index, model.Name)

                    enabled: root.currentPlayIndex != index || !root.playing
                }
            }

            MouseArea {
                id: mouseInput
                anchors.fill: parent
                hoverEnabled: true

                property bool isPressed: false
                property bool rewind: false
                property bool isMoved: false
                property int mouseStartX: 0
                property int mouseLastX: 0

                onPressed: {
                    isPressed = true;
                    isMoved = false
                    mouseStartX = mapToItem(root, mouseInput.mouseX, mouseInput.mouseY).x
                    mouseLastX = mouseStartX
                    collapseOthers(index)
                }
                onReleased: doRelease()
                onCanceled: doRelease()

                function doRelease() {
                    isPressed = false

                    if (controller.progress > 0.9 && !btnRemove.enabled) {
                        controller.completeToBeginning();
                        return;
                    }

                    if(!isMoved) {
                        if(controller.progress < 0.1)
                            root.chooseSong(index)
                        controller.completeToBeginning()
                    }
                    else {
                        if (rewind)
                            controller.completeToBeginning()
                        else
                            controller.completeToEnd()
                    }
                }

                onPositionChanged: {
                    if(!isPressed)
                        return

                    isMoved = true

                    var mouseX = mapToItem(root, mouseInput.mouseX, mouseInput.mouseY).x
                    var p = (mouseX - mouseStartX) / 100

                    // p < 0 : right to left
                    // prevent restarting controller when it's stabled
                    if ((p < 0 && controller.progress == 1) || (p >= 0 && controller.progress == 0))
                        return
                    p = p > 1 ? 1 : (p < -1 ? -1 : p)

                    controller.progress = p < 0 ? -p : 1 - p

                    if (controller.progress < 0.5)
                        rewind = true
                    if (controller.progress >= 0.5)
                        rewind = false

                    mouseLastX = mouseX
                }
                onEntered: item.hovered = true
                onExited: item.hovered = false
            }

            AnimationController {
                id: controller
                animation: ParallelAnimation {
                    NumberAnimation { target: item; property: "x"; duration: 200; from: 0; to: -100; easing.type: Easing.InOutCubic }

                    ColorAnimation {
                        target: mask
                        property: "color"
                        from: Qt.rgba(1, 1, 1, 0)
                        to: Qt.rgba(1, 0.5, 0.5, 0.5)
                        duration: 200
                    }
                }

                onProgressChanged: {
                    if (progress == 1 && !btnRemove.enabled && !mouseInput.isPressed) {
                        controller.completeToBeginning();
                    }
                }
            }

            Component.onCompleted: root.onCollapseOthers.connect(collapseSelf)
            Component.onDestruction: root.onCollapseOthers.disconnect(collapseSelf)

            function collapseSelf(){
                if(root.currentFocusIndex != index)
                    controller.completeToBeginning()
            }
        }

        footer: Rectangle {
            width: parent.width
            height: 100
            color: "transparent"
        }
    }
}

