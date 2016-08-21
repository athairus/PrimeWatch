import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Window {
    visible: true;
    width: 500;
    height: 500;

    // The states the app can be in
    StateGroup {
        id: primeWatch;
        state: "WaitForInput";
        states: [
            State {
                name: "WaitForInput";
                PropertyChanges {
                    target: welcomeScreen;
                    enabled: true;
                    opacity: 1;
                }

                PropertyChanges {
                    target: connectButton;
                    visible: true;
                }
            },

            State {
                name: "WaitForConnection";
                PropertyChanges {
                    target: welcomeScreen;
                    enabled: true;
                    opacity: 1;
                }

                PropertyChanges {
                    target: cancelButton;
                    visible: true;
                }
            },

            State {
                name: "Active";
                PropertyChanges {
                    target: mainArea;
                    enabled: true;
                    opacity: 1;
                }
            }
        ]

        property int transitionDuration: 250;

        // Interpolate property changes
        transitions: Transition {
            id: defaultTransition;
            PropertyAnimation { property: "opacity"; duration: primeWatch.transitionDuration; }
            PropertyAnimation { property: "scale"; duration: primeWatch.transitionDuration; }
        }
    }

    // Automatic state transitions
    Item {

        // Reset cancel button and go to main screen once connection is established
        Item {
            id: connectedChecker;

            Connections {
                target: client;
                onConnected: {
                    cancelButton.enabled = true;
                    primeWatch.state = "Active";
                }
            }
        }

        // Reset cancel button and return to welcome screen once connection is lost
        Item {
            id: disconnectedChecker;

            Connections {
                target: client;
                onDisconnected: {
                    cancelButton.enabled = true;
                    primeWatch.state = "WaitForInput";
                }
            }
        }
    }

    // The actual UI
    Item {
        anchors.fill: parent;

        ColumnLayout {
            id: welcomeScreen;
            anchors.margins: 10;
            anchors.centerIn: parent;

            enabled: false;
            opacity: 0;
            visible: opacity === 1 ? true : false;

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                text: client.statusText;
            }

            RowLayout {
                Layout.fillWidth: true;
                anchors.horizontalCenter: parent.horizontalCenter;

                // Dummy to balance out layout
                BusyIndicator { scale: 0.5; running: false; }

                Text {
                    text: "IP:";
                }

                TextField {
                    id: ipField;
                    text: "192.168.1.100";
                }

                Text {
                    text: ":43673";
                }

                Item { width: 10; height: width; } // Spacer

                Button {
                    id: connectButton;
                    text: "Connect";
                    visible: false;

                    onClicked: {
                        primeWatch.state = "WaitForConnection";
                        client.connectToServer( ipField.text );
                        //timer.start();
                    }

                    // FIXME: DELETE
                    Timer {
                        id: timer
                        interval: 1000;
                        running: false;
                        repeat: false;
                        onTriggered: {
                            cancelButton.enabled = true;
                            primeWatch.state = "Active";
                        }
                    }
                }

                Button {
                    id: cancelButton;
                    text: "Cancel";
                    visible: false;

                    onClicked: {
                        enabled = false;
                        client.abortConnection();
                    }
                }

                BusyIndicator {
                    scale: 0.5;
                    running: primeWatch.state === "WaitForConnection";
                }
            }
        }

        ColumnLayout {
            id: mainArea;

            enabled: false;
            opacity: 0;
            visible: opacity === 1 ? true : false;

            anchors.margins: 10;
            anchors.centerIn: parent;

            Text { text: "Game"; }
            Text { text: client.gameid + client.makerid; }

            Item { width: 10; height: width; } // Spacer

            Text { text: "Speed (X, Y, Z)"; }
            Text { text: client.speedX.toFixed( 2 ); }
            Text { text: client.speedY.toFixed( 2 ); }
            Text { text: client.speedZ.toFixed( 2 ); }

            Item { width: 10; height: width; } // Spacer

            Text { text: "Position (X, Y, Z)"; }
            Text { text: client.posX.toFixed( 2 ); }
            Text { text: client.posY.toFixed( 2 ); }
            Text { text: client.posZ.toFixed( 2 ); }

            Item { width: 10; height: width; } // Spacer

            Text { text: "World ID, status"; }
            Text { text: client.worldID; }
            Text { text: {
                    switch( client.worldStatus ) {
                        case 0: return "Loading";
                        case 1: return "Loading Map";
                        case 2: return "Loading Map Areas";
                        case 3: return "Loading Sky Box";
                        case 4: return "Loading Sound Groups";
                        case 5: return "Ready";
                        default: return "ERROR";
                    }
                }
            }

            Item { width: 10; height: width; } // Spacer

            Text { text: "Room"; }
            Text { text: client.room; }

            Item { width: 10; height: width; } // Spacer

            Text { text: "Health"; }
            Text { text: client.health.toFixed( 2 ); }

            Item { width: 10; height: width; } // Spacer

            Text { text: "Timer"; }
            Text { text: client.timer; }
        }
    }
}
