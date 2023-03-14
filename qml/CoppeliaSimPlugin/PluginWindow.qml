import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: mainWindow
    width: 320
    height: 225
    flags: (
        Qt.platform.os === "windows"
            ? Qt.Dialog | (applicationActive ? Qt.WindowStaysOnTopHint : 0) | Qt.MSWindowsFixedSizeDialogHint | Qt.WindowTitleHint | Qt.WindowSystemMenuHint
            : Qt.Tool
        )
    color: palette.window
    title: qsTr("QML Plugin Window")
    property alias simBridge: simBridge
    property bool sticky: false
    property alias palette: systemPalette
    property bool currentSceneActive: true
    property bool visibleInCurrentScene: true
    visible: visibleInCurrentScene && (sticky || currentSceneActive)
    property bool applicationActive: true

    SystemPalette {
        id: systemPalette
        colorGroup: SystemPalette.Active
    }

    CoppeliaSimBridge {
        id: simBridge
        onEventReceived: {
            if(typeof mainWindow[name] === 'function')
                mainWindow[name].call(mainWindow, JSON.parse(data))
        }
    }

    /* This event is sent by the plugin when the scene that has created
     * this window becomes active/inactive.
     */
    function onInstanceSwitch(active) {
        currentSceneActive = active
    }

    function onAppSwitch(active) {
        applicationActive = active
    }
}

