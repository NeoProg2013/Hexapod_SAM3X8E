import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
	visible: true
	width: 585
	height: 515
	color: "#000000"

	Connections {
		target: CppCore
		onConfigurationFound: {
			configurationList.append({
										 "name": configurationName
									 })
		}
		onShowActionMessage: {

			var color = "#FFFFFF"
			actionsList.append({
								   "message": message,
								   "messageColor": color,
								   "result": ""
							   })
		}
		onShowActionResult: {

			var color = "#FFFFFF"
			if (result == "OK") {
				color = "#00FF00"
			} else if (result == "FAIL") {
				color = "#FF0000"
			}

			actionsList.get(actionsList.rowCount() - 1).messageColor = color
			actionsList.get(actionsList.rowCount() - 1).result = result
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

	Pane {
		focusPolicy: Qt.StrongFocus
		anchors.fill: parent

		Label {
			x: 255
			width: 305
			height: 20
			text: qsTr("Ход выполнения операций")
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignHCenter
		}

		Label {
			width: 250
			height: 20
			text: qsTr("Список доступных конфигураций")
			horizontalAlignment: Text.AlignHCenter
		}

		Frame {
			y: 25
			width: 250
			height: 300

			ListView {
				id: configurationListView
				clip: true
				anchors.fill: parent
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				ScrollBar.vertical: ScrollBar {
					policy: ScrollBar.AsNeeded
				}
				delegate: Rectangle {
					clip: true
					width: parent.width - 25
					height: 40
					color: (index == configurationListView.currentIndex) ? "#888888" : "#000000"

					MouseArea {
						anchors.fill: parent
						onClicked: {
							configurationListView.currentIndex = index
						}
					}
					Label {
						x: 15
						width: parent.width - 15
						height: parent.height
						text: name
						color: "#FFFFFF"
						verticalAlignment: Qt.AlignVCenter
						horizontalAlignment: Qt.AlignLeft
					}
				}
				model: ListModel {
					id: configurationList
				}
			}
		}

		Button {
			y: 330
			width: 250
			height: 50
			text: qsTr("Обновить список конфигураций")
			font.pointSize: 10
			onClicked: {
				configurationList.clear()
				actionsList.clear()
				CppCore.requestConfigurationListFromServer()
			}
		}

		Button {
			y: 385
			width: 250
			height: 50
			enabled: configurationList.count != 0
			text: qsTr("Скачать конфигурацию")
			font.pointSize: 10
			onClicked: {
				actionsList.clear()
				var name = configurationList.get(
							configurationListView.currentIndex).name
				CppCore.requestConfigurationFileFromServer(name)
			}
		}

		Button {
			y: 440
			width: 250
			height: 50
			text: qsTr("Загрузить в устройство")
			font.pointSize: 10
			onClicked: {
				actionsList.clear()
				CppCore.loadConfigurationToDevice()
			}
		}

		Frame {
			x: 255
			y: 25
			width: 305
			height: 443
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
					Label {
						anchors.right: parent.right
						anchors.rightMargin: 30
						width: 40
						height: parent.height
						text: result
						color: messageColor
						font.pointSize: 10
						verticalAlignment: Qt.AlignVCenter
						horizontalAlignment: Qt.AlignRight
					}
				}
				model: ListModel {
					id: actionsList
				}
			}
		}

		Label {
			id: label
			x: 484
			y: 473
			text: qsTr("Версия 1.00")
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignRight
		}
	}
}
