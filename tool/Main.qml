pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami

// TODO: split all this into different files good God
Window {
    id: root

    minimumWidth: 800
    minimumHeight: 400
    width: 1000
    height: 600

    title: "Qmsstyles Tool - " + (backend.loaded ? backend.currentStylePath : "No msstyles loaded")

    visible: true

    QmsstylesBackend {
        id: backend
        imageFileItem: imageFileItem
        currentElement: treeView.selectionModel.currentIndex
    }

    FileDialog {
        id: openDialog

        nameFilters: [ "Windows Visual Style (*.msstyles)" ]
        acceptLabel: "Open"
        onAccepted: {
            backend.load(selectedFile);
            statusBar.showMessage("Loaded visual style: " + backend.currentStyleName);
        }
    }

    FileDialog {
        id: extractDialog

        fileMode: FileDialog.SaveFile
        nameFilters: [ "Image files (*.png *.jpg *.jpeg)" ]
        acceptLabel: "Extract"
        defaultSuffix: ".png"
        currentFile: imageFileItem.name
        onAccepted: {
            imageFileItem.extractTo(selectedFile);
        }
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.minimumHeight: 32

            BorderImage {
                anchors.fill: parent

                border {
                    left: 1
                    right: 1
                    top: 1
                    bottom: 1
                }
                source: "qrc:/res/commandbar.png"
            }

            RowLayout {
                component CommandButton: MouseArea {
                    id: controlRoot

                    property alias text: lbl.text
                    property alias icon: icon.source

                    implicitWidth: contentRow.implicitWidth + 14
                    implicitHeight: 24

                    hoverEnabled: true

                    Item {
                        id: contents

                        anchors.fill: parent

                        BorderImage {
                            anchors.fill: parent

                            border {
                                top: 3
                                bottom: 3
                                right: 3
                                left: 3
                            }

                            source: "qrc:/res/cmdbtn" + (!controlRoot.pressed ? "-hover" : "-pressed") + ".png"

                            visible: controlRoot.containsMouse || controlRoot.pressed
                        }

                        RowLayout {
                            id: contentRow

                            anchors.fill: parent
                            anchors.rightMargin: 6
                            anchors.leftMargin: 6

                            spacing: 2

                            Kirigami.Icon {
                                id: icon

                                implicitWidth: 16
                                implicitHeight: implicitWidth

                                opacity: controlRoot.enabled ? 1.0 : 0.5
                            }

                            Text {
                                id: lbl

                                color: "white"

                                visible: text !== ""
                                opacity: controlRoot.enabled ? 1.0 : 0.5
                            }
                        }
                    }
                }

                anchors.fill: parent
                anchors.leftMargin: 3
                anchors.rightMargin: 3

                spacing: 4

                CommandButton {
                    icon: "document-open"
                    text: "Open"
                    onClicked: openDialog.open();
                }

                CommandButton {
                    icon: "insert-image"
                    text: "Extract image"
                    enabled: imageFileItem.hasImageFile
                    onClicked: extractDialog.open();
                }

                Item { Layout.fillWidth: true }
            }
        }

        RowLayout {
            id: mainContents

            component Separator: Item {
                Layout.minimumWidth: 2
                Layout.fillHeight: true

                Rectangle {
                    width: 1
                    height: parent.height
                    color: "#a7bac5"
                }

                Rectangle {
                    x: 1
                    width: 1
                    height: parent.height
                    color: "white"
                }
            }

            component Header: BorderImage {
                property alias text: lbl.text

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                height: 24

                border {
                    left: 2
                    right: 2
                    top: 0
                    bottom: 2
                }
                source: "qrc:/res/header.png"

                Text {
                    id: lbl

                    anchors.fill: parent

                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 6
                }
            }

            spacing: 0

            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                id: structureContainer

                Layout.preferredWidth: 300
                Layout.fillHeight: true

                Header { text: "Structure View" }

                QQC2.ScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 24

                    contentWidth: treeView.contentWidth
                    contentHeight: treeView.contentHeight

                    TreeView {
                        id: treeView

                        width: parent.width

                        clip: true
                        alternatingRows: false
                        interactive: false
                        selectionModel: ItemSelectionModel {}
                        model: backend.structureModel
                        reuseItems: false
                        delegate: QQC2.TreeViewDelegate {
                            implicitWidth: treeView.width
                            implicitHeight: 16
                        }
                    }
                }
            }
            Separator {  }

            Item {
                id: previewContainer

                Layout.fillWidth: true
                Layout.fillHeight: true

                Header { text: "Preview" }

                Item {
                    anchors.fill: parent
                    anchors.topMargin: 24

                    Image {
                        anchors.fill: parent

                        source: "qrc:/res/transparentboxesthing.png"
                        fillMode: Image.Tile
                    }

                    QQC2.ScrollView {
                        id: imageScrollView

                        anchors.fill: parent

                        contentWidth: imageFileContainer.implicitWidth
                        contentHeight: imageFileContainer.implicitHeight

                        // must be inside a container to be able to center properly.
                        // can't center imagefile without this for whatever reason i might not know :/
                        Item {
                            id: imageFileContainer

                            implicitWidth: imageFileItem.implicitWidth > imageScrollView.width ? imageFileItem.implicitWidth : imageScrollView.width
                            implicitHeight: imageFileItem.implicitHeight > imageScrollView.height ? imageFileItem.implicitHeight : imageScrollView.height

                            // TODO: add stretching and slicing rect marking
                            ImageFileItem {
                                id: imageFileItem
                                anchors.centerIn: parent
                            }
                        }
                    }
                }
            }
            Separator {  }

            Item {
                id: propertiesContainer

                Layout.preferredWidth: 250
                Layout.fillHeight: true

                Header { text: "Properties View" }

                QQC2.ScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 24

                    contentWidth: listView.contentWidth
                    contentHeight: listView.contentHeight

                    TableView {
                        id: listView

                        width: parent.width

                        model: backend.propertiesModel
                        interactive: false
                        clip: true
                        reuseItems: false
                        delegate: Item {
                            id: delegateRoot

                            required property var display

                            implicitWidth: TableView.view.width / 2
                            implicitHeight: 19

                            clip: true

                            Text {
                                anchors.fill: parent
                                leftPadding: 4
                                rightPadding: 4
                                text: delegateRoot.display
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: statusBar

            Layout.fillWidth: true
            Layout.preferredHeight: 24

            function showMessage(message: string) {
                statusText.text = message;
                messageDelay.restart();
            }

            Timer {
                id: messageDelay
                interval: 3000
                onTriggered: statusText.text = "";
            }

            BorderImage {
                anchors.fill: parent
                border {
                    left: 1
                    right: 1
                    top: 2
                    bottom: 1
                }
                source: "qrc:/res/statusbar.png"
            }

            RowLayout {
                component SBSeparator: Item {
                    Layout.topMargin: 2
                    Layout.bottomMargin: 1
                    Layout.minimumWidth: 1
                    Layout.maximumWidth: 1
                    Layout.fillHeight: true

                    Rectangle {
                        anchors.fill: parent
                        color: "#d7d7d7"
                    }
                }

                anchors {
                    fill: parent
                    leftMargin: 3
                    rightMargin: 3
                }

                spacing: 6

                Text {
                    Layout.alignment: Qt.AlignVCenter
                    text: backend.structureModel.count + " classes"
                }
                SBSeparator {  }
                Text {
                    id: statusText
                    Layout.alignment: Qt.AlignVCenter
                }
                SBSeparator {  }

                Item { Layout.fillWidth: true }

                SBSeparator {  }
                Text {
                    id: versionText
                    Layout.alignment: Qt.AlignVCenter
                    text: "Targetting: " + (backend.currentStyleVersion ?? "Unknown")
                }
            }
        }
    }
}
