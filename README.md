# Qt QML plugin for CoppeliaSim

### Compiling

1. Install required packages for [libPlugin](https://github.com/CoppeliaRobotics/libPlugin): see libPlugin's README
2. Download and install Qt (same version as CoppeliaSim)
3. Checkout and compile
```
$ git clone --recursive https://github.com/CoppeliaRobotics/simExtQML.git
$ mkdir simExtQML/build && cd simExtQML/build
$ cmake ..
$ cmake --build .
```
You may need to set the `CMAKE_PREFIX_PATH` environment variable to the `lib/cmake` subdirectory of your Qt installation, i.e. `/path/to/Qt/Qt5.9.0/5.9/<platform>/lib/cmake`

You may need to run the deploy tool to copy additional Qt dependencies to CoppeliaSim:
```
macdeployqt $COPPELIASIM_ROOT_DIR/../.. -qmldir=/path/to/simExtQML/qml -always-overwrite -verbose=2
```

### Usage

In order to load QML components, a QML engine must be created:

```
engine=simQML.createEngine()
simQML.load(engine,'/path/to/Component.qml')
```

When the engine is not needed anymore, it should be destroyed with:

```
simQML.destroyEngine(engine)
```

Note: destroying the engine would destroy also all components created within that engine, e.g. all the windows.

It is also possible to load QML inline code, using `simQML.loadData`.

#### `PluginWindow` component

This component can be found in the `CoppeliaSimPlugin` QML module.

`PluginWindow` is a subclass of `QtQuick.Window` which sets some convenience properties and handlers.

It is possible to use any other component, such as `QtQuick.Window` as the top-level component, but `PluginWindow` is what makes sense most of the times.

Example:

```
simQML.loadData(engine,[[
    import QtQuick 2.15
    import CoppeliaSimPlugin 1.0

    PluginWindow {
        id: mainWindow
        width: 400
        height: 300
        title: qsTr("QML plugin window")
    }
]])
```

#### Communication: QML -> Lua

It is possible to send *events* from QML to Lua using the `sendEvent` (or `sendEventRaw`) method of the `CoppeliaSimBridge` component.

For convenience, `CoppeliaSimBridge` is already instantiated in the `PluginWindow` component and exposed as the property `simBridge`.

QML can call `simBridge.sendEvent(eventName, eventData)` to send an event to Lua.

In order to receive the event in Lua, an event handler must be registered with `simQML.setEventHandler`:

```
function myEventHandler(engine,eventName,eventData)
    print('received event',eventName,eventData)
end

engine=simQML.createEngine()
simQML.setEventHandler(engine,'myEventHandler')
simQML.loadData(engine,[[
    import QtQuick 2.15
    import CoppeliaSimPlugin 1.0

    PluginWindow {
        id: mainWindow
        width: 400
        height: 300
        title: qsTr("QML plugin window")

        Button {
            anchors.fill: parent
            text: "Click me"
            onClicked: mainWindow.simBridge.sendEvent('click', {someData: 42})
        }
    }
]])
```

#### Communication: Lua -> QML

It is possible to send *events* from Lua to QML using the `simQML.sendEvent(engine,name,data)` Lua function.

In order to receive the event in QML, the handler `onEventReceived` of the `CoppeliaSimBridge` object must be implemented.

For convenience, if using the `PluginWindow` component, that handler is already implemented, and the event will be dispatched as a call to a function defined in the `PluginWindow` component.

```
engine=simQML.createEngine()
simQML.loadData(engine,[[
    import QtQuick 2.15
    import CoppeliaSimPlugin 1.0

    PluginWindow {
        id: mainWindow
        width: 400
        height: 300
        title: qsTr("QML plugin window")

        function myEvent(data) {
            console.log("received event 'myEvent', someData=" + data.someData)
        }
    }
]])
simQML.sendEvent(engine,'myEvent',{someData=42})
```

#### Built-in events

An `onInstanceSwitch` event with bool argument is sent each time the scene owning the engine becomes active/inactive.

The `PluginWindow` component already implements the handler for this event, setting the `visible` property of the `Window`. Set the `sticky` property to `true` to override this behavior.
