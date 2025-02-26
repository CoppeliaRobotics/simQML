import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml 2.15

Window {
    id: mainWindow
    width: 320
    height: 225
    /*
     * On macOS we need Qt.Tool to have the window stay on top of the CoppeliaSim's main window.
     * On Windows it seems there is no way for that to happen, and we need a nasty hack (see
     * below, in all the places where simBridge.raiseAboveMainWindow is called).
     */
    readonly property int defaultFlags: (0
        | (Qt.platform.os === "windows"
            ? (0
                | Qt.CustomizeWindowHint
                | Qt.Dialog
                | Qt.WindowTitleHint
              )
            : Qt.Tool
          )
        | (closeable ? Qt.WindowCloseButtonHint : 0)
        | (resizable ? 0 : Qt.MSWindowsFixedSizeDialogHint)
    )
    flags: defaultFlags
    color: palette.window
    title: qsTr("QML Plugin Window")
    property alias simBridge: simBridge
    property bool sticky: false
    property alias palette: systemPalette
    property bool currentSceneActive: true
    property bool visibleInCurrentScene: true
    readonly property bool computedVisibility: visibleInCurrentScene && (sticky || currentSceneActive)
    property bool applicationActive: true
    property bool resizable: true
    property bool closeable: true

    onComputedVisibilityChanged: visible = computedVisibility

    SystemPalette {
        id: systemPalette
        colorGroup: SystemPalette.Active
    }

    CoppeliaSimBridge {
        id: simBridge
        onEventReceived: (name, data) => {
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
        if(Qt.platform.os === "windows" && active)
            simBridge.raiseAboveMainWindow(mainWindow)
    }

    function onActiveWindowChange() {
        if(Qt.platform.os === "windows")
            simBridge.raiseAboveMainWindow(mainWindow)
    }

    Timer {
        interval: 2500
        running: Qt.platform.os === "windows"
        repeat: true
        onTriggered: simBridge.raiseAboveMainWindow(mainWindow)
    }
}

