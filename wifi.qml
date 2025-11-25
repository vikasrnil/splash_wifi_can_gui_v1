import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.VirtualKeyboard 2.4
import QtQuick.VirtualKeyboard.Settings 2.4

Rectangle {
    anchors.fill: parent
    color: "#E3F2FD"   // Light pastel background
    border.color: "#1565C0"
    border.width: 6
    radius: 12

    property bool wifiOn: wifi.wifiOn
    property bool wifiConnected: wifi.connected
    property string connectedSSID: wifi.getConnectedSSID()
    property string ipAddr: wifi.getIpAddress()
    property bool showWifiList: false
    property string selectedSSID: ""
    
    Timer {
    interval: 1500
    running: true
    repeat: true
    onTriggered: {
        wifiConnected = wifi.connected
        connectedSSID = wifi.getConnectedSSID()
        ipAddr = wifi.getIpAddress()
    }
}

    

    // Back Button
    Button {
        id: backButton
        text: "← Back"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 12
        font.bold: true
        font.pixelSize: 18
        contentItem: Text {
            text: backButton.text
            font.bold: true
            font.pixelSize: 18
            color: "#1565C0"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        background: Rectangle {
            color: "#BBDEFB"
            border.color: "#1565C0"
            radius: 8
        }
        onClicked: stackView.pop()
    }

    Column {
        anchors.fill: parent
        spacing: 16
        padding: 20
        anchors.topMargin: 60

        // Title
        Text {
            text: "📶 WiFi Manager"
            font.pixelSize: 34
            font.bold: true
            color: "#0D47A1"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // WiFi status
        Row {
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                width: 18
                height: 18
                radius: 9
                border.color: "#333"
                border.width: 1
                color: !wifiOn ? "gray" : (wifiConnected ? "#4CAF50" : "#F44336")
            }

            Text {
                text: !wifiOn
                      ? "WiFi Off"
                      : (wifiConnected
                         ? "Connected to " + connectedSSID + " (" + ipAddr + ")"
                         : "Disconnected")
                color: wifiConnected ? "#4CAF50" : "#F44336"
                font.bold: true
                font.pixelSize: 20
            }
        }

        // WiFi toggle
        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Text { 
                text: "WiFi:"
                font.bold: true
                font.pixelSize: 20
                color: "#333"
            }

            Switch {
                checked: wifiOn
                onToggled: {
                    wifiOn = checked
                    wifi.wifiOn = checked
                    if (!checked) showWifiList = false
                }
            }
        }

        // Show WiFi list button
        Button {
            text: showWifiList ? "Hide WiFi List" : "Show WiFi List"
            width: 260
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 18
            background: Rectangle {
                color: "#4CAF50"
                radius: 10
                border.color: "#2E7D32"
                border.width: 2
            }
            contentItem: Text {
                text: parent.text
                font.bold: true
                font.pixelSize: 18
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                showWifiList = !showWifiList
                if (showWifiList) {
                    wifi.scanWifi()
                    console.log("WiFi scan triggered.")
                }
            }
        }

        // WiFi list view
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 80
            height: showWifiList ? 240 : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            color: "#FFFFFF"
            radius: 12
            border.color: "#90CAF9"
            border.width: 2
            visible: showWifiList
            clip: true

            ListView {
                id: wifiListView
                anchors.fill: parent
                model: ListModel {}

                delegate: Rectangle {
                    width: parent.width
                    height: 40
                    radius: 8
                    color: index % 2 === 0 ? "#E3F2FD" : "#BBDEFB"
                    border.color: "#64B5F6"
                    border.width: 1

                    Row {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Text { 
                            text: name
                            width: parent.width * 0.8
                            font.bold: true
                            font.pixelSize: 18
                            color: "#0D47A1"
                        }

                        Rectangle {
                            width: 14; height: 14; radius: 7
                            color: name === connectedSSID ? "#4CAF50" : "transparent"
                            border.color: "#333"
                            border.width: 1
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            selectedSSID = name
                            passField.text = ""
                            pwdPopup.open()
                        }
                        onEntered: parent.color = "#64B5F6"
                        onExited: parent.color = index % 2 === 0 ? "#E3F2FD" : "#BBDEFB"
                    }
                }
            }
        }
    }

    // Password Popup

Dialog {
    id: pwdPopup
    modal: true
    title: "Connect to " + selectedSSID
    standardButtons: Dialog.Ok | Dialog.Cancel
    anchors.centerIn: parent

    // Open focus to show virtual keyboard
    onOpened: {
        Qt.inputMethod.show()           // << FORCE OPEN KEYBOARD
        passField.forceActiveFocus()    // << FOCUS TEXTFIELD
    }

    Column {
        spacing: 12
        padding: 20
        anchors.horizontalCenter: parent.horizontalCenter

        // Password field
        TextField {
            id: passField
            placeholderText: "Password"
            echoMode: TextInput.Password
            width: 260

            onActiveFocusChanged: {
                if (activeFocus)
                    Qt.inputMethod.show()    // << IF USER CLICKS FIELD
            }
        }

        // Show / Hide password
        CheckBox {
            text: "Show Password"
            onCheckedChanged: {
                passField.echoMode = checked ? TextInput.Normal : TextInput.Password
            }
        }
    }
}



    // Handle scan results
    Connections {
        target: wifi
        onWifiScanCompleted: function(list){
            wifiListView.model.clear()
            for (var i = 0; i < list.length; i++) {
                wifiListView.model.append({ name: list[i] })
            }
        }
    }
}

