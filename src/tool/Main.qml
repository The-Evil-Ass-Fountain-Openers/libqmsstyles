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

    title: {
        var titleStr = "Qmsstyles Tool - ";
        if (backend.loaded) {
            titleStr += backend.currentStyleName + " (" + backend.currentStyleVersion + ")";
        } else {
            titleStr += "No visual style loaded";
        }
    }

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
        }
    }

    FileDialog {
        id: extractDialog

        fileMode: FileDialog.SaveFile
        nameFilters: [ "Image files (*.png *.jpg *.jpeg *.bmp)" ]
        acceptLabel: "Extract"
        defaultSuffix: backend.currentStyleVersion < 2 ? ".bmp" : ".png"
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
                property Item resizeTarget: parent

                implicitWidth: 2
                implicitHeight: parent.height

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

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.SizeHorCursor
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
                    anchors.rightMargin: 2

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

                Separator { anchors.right: parent.right }
            }

            Item {
                id: previewContainer

                Layout.fillWidth: true
                Layout.fillHeight: true

                Header { text: "Preview" }

                Item {
                    anchors.fill: parent
                    anchors.topMargin: 24

                    QQC2.ScrollView {
                        id: imageScrollView

                        anchors.fill: parent

                        contentWidth: imageFileContainer.implicitWidth
                        contentHeight: imageFileContainer.implicitHeight

                        Item {
                            id: imageFileContainer

                            implicitWidth: imageFileItem.implicitWidth > imageScrollView.width ? imageFileItem.implicitWidth : imageScrollView.width
                            implicitHeight: imageFileItem.implicitHeight > imageScrollView.height ? imageFileItem.implicitHeight : imageScrollView.height

                            Image {
                                anchors.fill: parent

                                source: "qrc:/res/transparentboxesthing.png"
                                fillMode: Image.Tile
                            }

                            // TODO: add stretching and slicing rect marking
                            ImageFileItem {
                                id: imageFileItem
                                anchors.centerIn: parent
                            }
                        }
                    }
                }
            }

            Item {
                id: propertiesContainer

                Layout.preferredWidth: 250
                Layout.fillHeight: true

                Header { text: "Properties View" }

                QQC2.ScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 24
                    anchors.leftMargin: 2

                    contentWidth: tableView.contentWidth
                    contentHeight: tableView.contentHeight

                    TableView {
                        id: tableView

                        contentWidth: propertiesContainer.width - 2

                        model: backend.propertiesModel
                        interactive: false
                        clip: true
                        reuseItems: false
                        selectionMode: TableView.SingleSelection
                        editTriggers: TableView.DoubleTapped
                        delegateModelAccess: DelegateModel.ReadWrite
                        columnSpacing: 1
                        delegate: QQC2.TableViewDelegate {
                            id: delegateRoot

                            implicitWidth: (TableView.view.width / 2) - 1

                            // the KDE desktop QQC2 sets some nasty defaults
                            leftPadding: 0
                            rightPadding: 0
                            topPadding: 0
                            bottomPadding: 0

                            leftInset: 0
                            rightInset: 0
                            topInset: 0
                            bottomInset: 0

                            contentItem: Text {
                                leftPadding: 2
                                rightPadding: 2
                                topPadding: 1
                                bottomPadding: 1

                                text: delegateRoot.model.name ?? ""
                                elide: Text.ElideRight
                            }

                            TableView.editDelegate: QQC2.TextField {
                                width: delegateRoot.width
                                height: delegateRoot.height

                                leftPadding: 2
                                rightPadding: 2
                                topPadding: 1
                                bottomPadding: 1

                                // TODO: make the model pass the validator
                                /*validator: RegularExpressionValidator {
                                    // longest regex i've done in my entire life
                                    regularExpression: /(?:(?:\d{1,}(?:(?:(?:\, )|(?:\,))))|\d{1,}$){4,4}/
                                }*/
                                color: acceptableInput ? "black" : "red"
                                text: delegateRoot.model.name ?? ""

                                QQC2.ToolTip.visible: delegateRoot.hovered && delegateRoot.model.formatHelp != "ignore"
                                QQC2.ToolTip.text: delegateRoot.model.formatHelp

                                TableView.onCommit: delegateRoot.model.name = text
                            }
                        }
                    }
                }

                Separator { anchors.left: parent.left }
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
                    text: backend.loaded ? backend.currentStylePath : ""
                }
            }
        }
    }
}
