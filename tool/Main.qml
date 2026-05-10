pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami

import qmsstyles
import qmsstyles.style as VisualStyle

Window {
    id: root

    property VisualStyle.Style currentStyle: null

    minimumWidth: 800
    minimumHeight: 400
    width: 1000
    height: 600

    title: "Qmsstyles Tool - " + (currentStyle ? (currentStyle.path ?? "Unknown") : "No msstyles loaded")

    visible: true

    FileDialog {
        id: fileDialog

        nameFilters: [ "Windows Visual Style (*.msstyles)" ]
        acceptLabel: "Open"
        onAccepted: {
            if (root.currentStyle) {
                Qmsstyles.unload(root.currentStyle);
            }

            root.currentStyle = Qmsstyles.load(currentFile);
            statusBar.showMessage("Loaded visual style: " + root.currentStyle.name);
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
                    onClicked: fileDialog.open();
                }

                CommandButton {
                    icon: "insert-image"
                    text: "Extract IMAGEFILE"
                    enabled: false
                }

                CommandButton {
                    icon: "list-add"
                    text: "Add class"
                    enabled: false
                }

                CommandButton {
                    icon: "list-remove"
                    text: "Remove class"
                    enabled: false
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

                Header { text: "Classes" }

                QQC2.ScrollView {
                    anchors.fill: parent
                    anchors.topMargin: 24

                    contentWidth: structureColumn.implicitWidth
                    contentHeight: structureColumn.implicitHeight

                    Column {
                        id: structureColumn

                        Repeater {
                            id: structureRepeater

                            model: root.currentStyle?.classes.length ?? 0
                            delegate: Item {
                                id: delegate

                                required property int index

                                readonly property var adjacentClass: root.currentStyle.classes[index]

                                property bool expanded: false

                                width: row.implicitWidth
                                height: 16

                                Row {
                                    id: row

                                    anchors.fill: parent

                                    Image {
                                        source: "qrc:/res/treearrow" + (delegate.expanded ? "-expanded" : "") + (arrowMa.containsMouse ? "-hover" : "-normal") + ".png"

                                        MouseArea {
                                            id: arrowMa
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onClicked: delegate.expanded = !delegate.expanded
                                        }
                                    }

                                    Text {
                                        id: label

                                        leftPadding: 4
                                        rightPadding: 4
                                        text: delegate.adjacentClass.name
                                        color: "black"
                                    }
                                }
                            }
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

                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 24
                    color: "black"
                }
            }

            Separator {  }

            Item {
                id: propertiesContainer

                Layout.preferredWidth: 200
                Layout.fillHeight: true

                Header { text: "Properties View" }
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
                    text: (root.currentStyle?.classes.length ?? 0) + " classes"
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
                    text: {
                        if (!root.currentStyle) {
                            return "Targetting: None";
                        }

                        switch (root.currentStyle.version) {
                            case VisualStyle.Style.Version.WindowsVista:
                                return "Targetting: Windows Vista";
                            case VisualStyle.Style.Version.Windows7:
                                return "Targetting: Windows 7";
                            case VisualStyle.Style.Version.Windows8:
                                return "Targetting: Windows 8/8.1";
                            case VisualStyle.Style.Version.Windows10:
                                return "Targetting: Windows 10";
                            case VisualStyle.Style.Version.Windows11:
                                return "Targetting: Windows 11";
                            default:
                                return "Targetting: Unknown";
                        }
                    }
                }
            }
        }
    }
}
