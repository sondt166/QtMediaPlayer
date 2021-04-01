import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import MediaPlayer 1.0
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 600
    height: 400
    title: qsTr("Music Player")

    color: Qt.rgba(0.1,0.1,0.1,1)

    property int dialogRemoveAwaitIndex: -1

    FileDialog {
        id: fileDialog
        title: "Please choose a folder"
        folder: shortcuts.home
        selectFolder: true
        onAccepted: {
            console.log("You chose: " + fileDialog.folder)
            songPlayer.loadFolder(fileDialog.folder.toString().replace("file:///", ""))
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    MyDialog {
        id: dialog
        title: "Confimation to remove song"
        onYes: {
            if (dialogRemoveAwaitIndex != -1)
                songList.removeItemAt(dialogRemoveAwaitIndex)
        }
        onNo: dialogRemoveAwaitIndex = -1
        onDiscard: dialogRemoveAwaitIndex = -1
    }

    SongListView {
        id: listView
        anchors.fill: parent

        onChooseSong: {
            songPlayer.chooseSong(index)
            listView.currentIndex = index
        }
        onRemoveSong: songRemove(index, title)

        playing: songPlayer.playing
    }

    ShaderEffectSource {
        id: effectSource

        sourceItem: listView
        anchors.fill: controls

        sourceRect: Qt.rect(x,y, width, height)
    }

    GaussianBlur {
        id: blur
        anchors.fill: effectSource

        source: effectSource
        radius: 16
        samples: 1 + radius * 2
    }

    Controls {
        id: controls
        height: 100
        width: parent.width
        anchors.bottom: parent.bottom
        playing: songPlayer.playing

        onSeeked: songPlayer.seek(value * songPlayer.currentDuration)
        onPlay: {
            songPlayer.play()
        }
        onPause: songPlayer.pause()
        onNext: {
            if (songPlayer.next()){
                listView.increseIndex()
            }
        }
        onPrev: {
            if (songPlayer.prev()){
                listView.decreaseIndex()
            }
        }
    }

    Connections {
        target: songPlayer
        onSongChanged: {
            window.title = songPlayer.currentName + " - " + songPlayer.currentArtist
            controls.duration = songPlayer.currentDuration
            listView.currentPlayIndex = songPlayer.currentSongIndex
        }
        onElapsedDutionChanged: {
            controls.elapsed = elapsed
        }
    }

    Item {
        focus: true
        onFocusChanged: focus = true
        Keys.onPressed: {
            if (event.key === Qt.Key_O && (event.modifiers & Qt.ControlModifier)){
                fileDialog.open()
            }
        }
    }

    function songRemove(index, title){
        dialogRemoveAwaitIndex = index;
        dialog.text = "Remove " + title + " ?"
        dialog.open()
    }
}
