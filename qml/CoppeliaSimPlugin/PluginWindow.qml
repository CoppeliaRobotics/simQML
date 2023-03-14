import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    id: mainWindow
    width: 320
    height: 225
    flags: (
        simBridge.platform == 0 /* Windows */
            ? Qt.Dialog | Qt.WindowStaysOnTopHint | Qt.MSWindowsFixedSizeDialogHint | Qt.WindowTitleHint
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
}

