import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 300
    height: 500
    color: "#000000"
    title: "SkynetSetup"

    minimumWidth: 300
    minimumHeight: 300

    Connections {
        target: CppCore
        onConfigurationFound: {
            configurationList.append({
                                         "name": configurationName
                                     })
        }
        onShowActionMessage: {

            var color = "#FFFF00"
            actionsList.append({
                                   "message": message,
                                   "messageColor": color,
                                   "result": ""
                               })
        }
        onShowActionResult: {

            var color = "#FFFF00"
            if (result == true) {
                color = "#00FF00"
            } else{
                color = "#FF0000"
            }

            actionsList.get(actionsList.rowCount() - 1).messageColor = color
        }
        onShowOperationCompletedMessage: {

            var color = "#00FF00"
            actionsList.append({
                                   "message": message,
                                   "messageColor": color,
                                   "result": ""
                               })
        }
    }

    Frame {
        x: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        anchors.top: parent.top
        anchors.topMargin: 110
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        ListView {
            id: logView
            anchors.fill: parent
            clip: true
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: Rectangle {
                width: parent.width
                height: 20
                color: "#000000"
                clip: true

                Label {
                    x: 5
                    width: parent.width - 80
                    height: parent.height
                    text: message
                    color: messageColor
                    font.pointSize: 10
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignVCenter
                }
            }
            model: ListModel {
                id: actionsList
            }
            onCountChanged: {
                logView.currentIndex = logView.count - 1
            }
        }
    }


    Button {
        id: searchDeviceButton
        height: 50
        text: qsTr("Найти устройство")
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        font.pointSize: 10
        onClicked: {
            actionsList.clear()

            searchDeviceButton.enabled = false
            loadButton.enabled = CppCore.findDevice()
            searchDeviceButton.enabled = true
        }
    }

    Button {
        id: loadButton
        height: 50
        enabled: false
        text: qsTr("Загрузить конфигурацию")
        anchors.top: parent.top
        anchors.topMargin: 55
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        font.pointSize: 10
        onClicked: {
            actionsList.clear()

            searchDeviceButton.enabled = false
            loadButton.enabled = false
            loadButton.enabled = CppCore.loadConfigurationToDevice()
            searchDeviceButton.enabled = true
        }
    }

    Label {
        x: 504
        y: 275
        height: 20
        text: qsTr("Версия 1.00")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
    }
}
