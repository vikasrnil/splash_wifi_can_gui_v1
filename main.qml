import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
    visible: true
    width: Screen.width
    height: Screen.height
    visibility: Window.FullScreen
    title: "Main Menu"

    // Flag to control splash visibility
    property bool showSplash: true

    // Splash Screen
    Rectangle {
        id: splashScreen
        anchors.fill: parent
        color: "#ffffff"   // White background for splash
        visible: showSplash
        border.color: "#1565C0"
        border.width: 20

        Image {
            anchors.centerIn: parent
            source: "qrc:/logo.png"   // Replace with your logo path
            fillMode: Image.PreserveAspectFit
            width: parent.width * 0.4
            height: parent.height * 0.4
        }

        Timer {
            interval: 5000   // 5 seconds
            running: true
            repeat: false
            onTriggered: showSplash = false
        }
    }

    // Main UI (hidden until splash disappears)
    StackView {
        id: stackView
        anchors.fill: parent
        visible: !showSplash   // Show only after splash ends

        initialItem: Rectangle {
            anchors.fill: parent
            color: "#f5f5f5"   // Light background
            border.color: "#FFE0B2"
            border.width: 20
            radius: 6

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 40
                Layout.alignment: Qt.AlignCenter

                // 🏠 Home Screen Title
                Text {
                    text: "🏠 Home Screen"
                    font.pixelSize: 40
                    font.bold: true
                    color: "#333333"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                }

                // 🚜 CAN Page icon
                Button {
                    text: "🚜 CAN Monitor"
                    font.pixelSize: 24
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                    width: 300
                    background: Rectangle {
                        color: "#e0f7fa"
                        radius: 10
                        border.color: "#00796B"
                        border.width: 2
                    }
                    onClicked: stackView.push("qrc:/CanPage.qml")
                }

                // 📶 WiFi Page icon
                Button {
                    text: "📶 WiFi Settings"
                    font.pixelSize: 24
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                    width: 300
                    background: Rectangle {
                        color: "#fff9c4"
                        radius: 10
                        border.color: "#FBC02D"
                        border.width: 2
                    }
                    onClicked: stackView.push("qrc:/Wifi.qml")
                }
            }
        }
    }
}

