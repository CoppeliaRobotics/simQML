#include "UI.h"
#include "Bridge.h"

#include <QThread>
//#include <QQuickStyle>

#include <simPlusPlus/Lib.h>

// UI is a singleton

UI *UI::instance = nullptr;
QWidget *UI::simMainWindow = nullptr;
float UI::wheelZoomFactor = 1.0;

UI::UI(QObject *parent)
    : QObject(parent)
{
    //QQuickStyle::setStyle("Fusion");
    qputenv("QT_QUICK_CONTROLS_STYLE", "Fusion");
}

UI::~UI()
{
    UI::instance = nullptr;
}

UI * UI::getInstance(QObject *parent)
{
    if(!UI::instance)
    {
        UI::instance = new UI(parent);
        UI::simMainWindow = (QWidget *)simGetMainWindow(1);
        simGetFloatParameter(sim_floatparam_mouse_wheel_zoom_factor, &UI::wheelZoomFactor);
        sim::addLog(sim_verbosity_debug, "UI(%x) constructed in thread %s", UI::instance, QThread::currentThreadId());
    }
    return UI::instance;
}

void UI::destroyInstance()
{
    if(UI::instance)
    {
        auto inst = UI::instance;
        disconnect(inst, nullptr, nullptr, nullptr);
        UI::instance->deleteLater();
        UI::instance = nullptr;
        sim::addLog(sim_verbosity_debug, "UI(%x) destroyed in thread %s", inst, QThread::currentThreadId());
    }
}

void UI::onCreateEngine(QQmlApplicationEngine **engine, const QStringList &importPaths)
{
    sim::addLog(sim_verbosity_debug, "creating a new QML engine...");
    auto e = new QQmlApplicationEngine(this);
    for(const auto &path : importPaths)
        e->addImportPath(path);
    sim::addLog(sim_verbosity_debug, "QML import path list: %s", e->importPathList().join(", ").toStdString());
    connect(e, &QQmlEngine::warnings, [=] (const QList<QQmlError> &warnings) {
        QVariant contextInfo = e->property("contextInfo");
        for(const auto &msg : warnings)
        {
            int v = 0;
            switch(msg.messageType())
            {
            case QtDebugMsg:
                v = sim_verbosity_debug;
                break;
            case QtInfoMsg:
                v = sim_verbosity_infos;
                break;
            case QtWarningMsg:
                v = sim_verbosity_warnings;
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                v = sim_verbosity_errors;
                break;
            }
            QString ctx;
            if(contextInfo.isValid())
                ctx += contextInfo.toString() + QStringLiteral(", ");
            if(msg.url().isEmpty())
                ctx += QStringLiteral("<inline-QML>");
            else
                ctx += msg.url().toLocalFile();
            sim::addLog(v, "QML [%s:%u]: %s", ctx.toLocal8Bit().constData(), msg.line(), msg.description().toLocal8Bit().constData());
        }
    });
    sim::addLog(sim_verbosity_debug, "created a new QML engine: 0x%x", e);
    *engine = e;
}

void UI::onDestroyEngine(QQmlApplicationEngine *engine)
{
    sim::addLog(sim_verbosity_debug, "destroying QML engine 0x%x", engine);
    engine->deleteLater();
}

void UI::onSetEventHandler(QQmlApplicationEngine *engine, int scriptID, QString functionName)
{
    if(!engine)
    {
        sim::addLog(sim_verbosity_errors, "received null engine pointer in UI::onSetEventHandler");
        return;
    }
    engine->setProperty("eventHandlerScriptID", scriptID);
    engine->setProperty("eventHandlerFunctionName", functionName);
}

void UI::onSetEngineHandle(QQmlApplicationEngine *engine, QString handle)
{
    if(!engine)
    {
        sim::addLog(sim_verbosity_errors, "received null engine pointer in UI::onSetEventHandler");
        return;
    }
    engine->setProperty("simHandle", handle);
}

void UI::onLoad(QQmlApplicationEngine *engine, QString filename, QString contextInfo)
{
    engine->setProperty("contextInfo", contextInfo);
    engine->load(QUrl(filename));
    engine->setProperty("contextInfo", QVariant());
}

void UI::onLoadData(QQmlApplicationEngine *engine, QByteArray data, QString basepath, QString contextInfo)
{
    engine->setProperty("contextInfo", contextInfo);
    engine->loadData(data, QUrl(basepath));
    engine->setProperty("contextInfo", QVariant());
}

void UI::onSetMeshData(Geometry *geom, QByteArray vertexData, QByteArray indexData)
{
    geom->setMeshData(vertexData, indexData);
}
