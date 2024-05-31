#include "Bridge.h"
#include "SIM.h"

#include <QApplication>
#include <QEvent>

#include <simPlusPlus/Lib.h>

void Bridge::registerQmlType()
{
    qmlRegisterType<Bridge>("CoppeliaSimPlugin.Bridge", 1, 0, "CoppeliaSimBridge");
}

Bridge::Bridge(QObject *parent)
    : QObject(parent)
{
    SIM *sim = SIM::getInstance();
    connect(this, &Bridge::eventFromQML, sim, &SIM::onEventFromQML);
    connect(sim, &SIM::sendEventToQML, this, &Bridge::eventFromSIM, Qt::BlockingQueuedConnection);
    QApplication::instance()->installEventFilter(this);
}

Bridge::~Bridge()
{
}

#include <iostream>

bool Bridge::eventFilter(QObject *object, QEvent *event)
{
    auto engine = static_cast<QQmlApplicationEngine *>(qmlEngine(this));
    QWidget *mainWindow = (QWidget *)sim::getMainWindow(1);
    switch(event->type())
    {
    case QEvent::ApplicationActivate:
        eventFromSIM(engine, "onAppSwitch", "true");
        break;
    case QEvent::ApplicationDeactivate:
        eventFromSIM(engine, "onAppSwitch", "false");
        break;
    case QEvent::ActivationChange:
        eventFromSIM(engine, "onActiveWindowChange", "null");
        break;
    }
    return false;
}

void Bridge::eventFromSIM(QQmlApplicationEngine *engine, QString name, QByteArray data)
{
    auto myEngine = static_cast<QQmlApplicationEngine *>(qmlEngine(this));
    if(myEngine != engine) return;

    sim::addLog(sim_verbosity_debug, "Bridge::eventFromSIM(\"%s\", \"%s\")", name.toStdString(), std::string(data.constData(), data.length()));
    emit eventReceived(name, data);
    /*
     * or we could simply use:
     *
     * QMetaObject::invokeMethod(engine->rootObjects().at(0), "functionName", Q_ARG(QVariant, QVariant::fromValue(QString("blabla"))));
     *
     * however handling variable number of arguments with arbitrary type still remains a problem.
     */
}

void Bridge::sendEventRaw(QString name, QByteArray data)
{
    sim::addLog(sim_verbosity_debug, "Bridge::sendEventRaw(\"%s\", \"%s\")", name.toStdString(), std::string(data.constData(), data.length()));
    auto engine = static_cast<QQmlApplicationEngine *>(qmlEngine(this));
    emit eventFromQML(engine, name, data);
}

void Bridge::raiseAboveMainWindow(QQuickWindow *window)
{
#ifdef WIN32
    QWidget *mainWindow = (QWidget *)sim::getMainWindow(1);
    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    HWND hwndMain = reinterpret_cast<HWND>(mainWindow->winId());
    // check if mainWindow is stacked above the window
    HWND hwndAfter = ::GetWindow(hwndMain, GW_HWNDNEXT);
    while(hwndAfter != nullptr) {
        if(hwndAfter == hwnd) {
            // if so, raise window above mainWindow
            HWND hwndInsertAfter = ::GetNextWindow(hwndMain, GW_HWNDPREV);
            ::SetWindowPos(hwnd, hwndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            break;
        }
        hwndAfter = ::GetWindow(hwndAfter, GW_HWNDNEXT);
    }
#endif
}
