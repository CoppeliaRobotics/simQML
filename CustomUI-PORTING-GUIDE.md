# CustomUI Porting Guide

This guide describes how to port old `simUI`-based code to `QML` / `QtQuick` for the `simQML` plugin.

## UI creation and destruction

One should use `simQML.load(engine,path)` (or `simQML.loadData(engine,data)`) to load a component in the QML engine.

The top-level component to use to create a window, that behaves similarly to `simUI`'s `<ui>` is `PluginWindow`.

There is no direct control of which windows are created, and it is not possible to destroy windows from Lua.

Instead, events should be sent via `simQML.sendEvent()`, and windows management should be handled in the QML engine (see [README.md](README.md)).


## `<ui>` attributes

`title` is supported by [`QtQuick.Window`](https://doc.qt.io/qt-5/qml-qtquick-window-window.html).

`modal` is [`QtQuick.Window`](https://doc.qt.io/qt-5/qml-qtquick-window-window.html)'s `modality`.

`resizable` and `closable` are convenience properties provided by QML type `PluginWindow`; the same is possible via [`QtQuick.Window`](https://doc.qt.io/qt-5/qml-qtquick-window-window.html)'s `flags`.

`style` is not supported: `QtQuick` don't use Qt stylesheets.

`layout` is not supported: `QtQuick.Layouts` should be used instead.

`content-margins` is not supported: use related `QtQuick` properties.

`placement`, `position` and `size` are not provided. Use property `x`, `y`, `width`, `height` of the window and of `Screen`.

`activate` is not supported: use a suitable [`QtQuick.Window`](https://doc.qt.io/qt-5/qml-qtquick-window-window.html)'s `flags`.

`on-close`: use the [`onClosing`](https://doc.qt.io/qt-5/qml-qtquick-window-window.html#closing-signal) handler of `PluginWindow` / [`QtQuick.Window`](https://doc.qt.io/qt-5/qml-qtquick-window-window.html).

## Widgets

`<button>`: use [`QtQuick.Controls.Button`](https://doc.qt.io/qt-5/qml-qtquick-controls2-button.html)

`<checkbox>`: use [`QtQuick.Controls.CheckBox`](https://doc.qt.io/qt-5/qml-qtquick-controls2-checkbox.html)

`<combobox>`: use [`QtQuick.Controls.ComboBox`](https://doc.qt.io/qt-5/qml-qtquick-controls2-combobox.html)

`<edit>`: use [`QtQuick.Controls.TextField`](https://doc.qt.io/qt-5/qml-qtquick-controls2-textfield.html)

`<group>`: use [`QtQuick.Controls.CheckBox`](https://doc.qt.io/qt-5/qml-qtquick-controls2-checkbox.html)

`<hslider>`: use [`QtQuick.Controls.Slider`](https://doc.qt.io/qt-5/qml-qtquick-controls2-slider.html)

`<image>`: use [`QtQuick.Image`](https://doc.qt.io/qt-5/qml-qtquick-image.html)

`<label>`: use [`QtQuick.Controls.Label`](https://doc.qt.io/qt-5/qml-qtquick-controls2-label.html)

`<plot>`: use [`QtQuick.Controls.CheckBox`](https://doc.qt.io/qt-5/qml-qtquick-controls2-checkbox.html)

`<progressbar>`: use [`QtQuick.Controls.ProgressBar`](https://doc.qt.io/qt-5/qml-qtquick-controls2-progressbar.html)

`<radiobutton>`: use [`QtQuick.Controls.RadioButton`](https://doc.qt.io/qt-5/qml-qtquick-controls2-radiobutton.html)

`<scene3d>`: use [`QtQuick.Scene3D`](https://doc.qt.io/qt-5/qml-qtquick-scene3d-scene3d.html)

`<spinbox>`: use [`QtQuick.Controls.SpinBox`](https://doc.qt.io/qt-5/qml-qtquick-controls2-spinbox.html)

`<table>`: use [`QtQuick.TableView`](https://doc.qt.io/qt-5/qml-qtquick-tableview.html)

`<tabs>`: use [`QtQuick.Controls.CheckBox`](https://doc.qt.io/qt-5/qml-qtquick-controls2-checkbox.html)

`<tab>`: use [`QtQuick.Controls.TabBar`](https://doc.qt.io/qt-5/qml-qtquick-controls2-tabbar.html)

`<text-browser>`: use [`QtQuick.Controls.TextArea`](https://doc.qt.io/qt-5/qml-qtquick-controls2-textarea.html)

`<tree>`: use [`QtQuick.TreeView`](https://doc.qt.io/qt-5/qml-qtquick-treeview.html)

`<vslider>`: use [`QtQuick.Controls.Slider`](https://doc.qt.io/qt-5/qml-qtquick-controls2-slider.html)

`<svg>`: use [`QtQuick.Image`](https://doc.qt.io/qt-5/qml-qtquick-image.html)

`<br>`, `<stretch>`: use QtQuick.Layouts system.
