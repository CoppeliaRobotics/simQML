#include "SIM.h"
#include "UI.h"
#include "stubs.h"

#include <QThread>

#include <simPlusPlus/Lib.h>

// SIM is a singleton

SIM *SIM::instance = nullptr;

SIM::SIM(QObject *parent)
    : QObject(parent)
{
    UI *ui = UI::getInstance();
    // connect signals/slots from UI to SIM and vice-versa
    connect(this, &SIM::createEngine, ui, &UI::onCreateEngine, Qt::BlockingQueuedConnection);
    connect(this, &SIM::destroyEngine, ui, &UI::onDestroyEngine, Qt::BlockingQueuedConnection);
    connect(this, &SIM::setEventHandler, ui, &UI::onSetEventHandler, Qt::BlockingQueuedConnection);
    connect(this, &SIM::setEngineHandle, ui, &UI::onSetEngineHandle, Qt::BlockingQueuedConnection);
    connect(this, &SIM::load, ui, &UI::onLoad, Qt::BlockingQueuedConnection);
    connect(this, &SIM::loadData, ui, &UI::onLoadData, Qt::BlockingQueuedConnection);

#ifdef Qt5_Quick3D_FOUND
    connect(ui, &UI::getMeshData, this, &SIM::onGetMeshData);
    connect(this, &SIM::updateMeshData, ui, &UI::onSetMeshData);
#endif // Qt5_Quick3D_FOUND
}

SIM::~SIM()
{
    SIM::instance = nullptr;
}

SIM * SIM::getInstance(QObject *parent)
{
    if(!SIM::instance)
    {
        SIM::instance = new SIM(parent);
        sim::addLog(sim_verbosity_debug, "SIM(%x) constructed in thread %s", SIM::instance, QThread::currentThreadId());
    }
    return SIM::instance;
}

void SIM::destroyInstance()
{
    if(SIM::instance)
    {
        auto inst = SIM::instance;
        disconnect(inst, nullptr, nullptr, nullptr);
        SIM::instance->deleteLater();
        SIM::instance = nullptr;
        sim::addLog(sim_verbosity_debug, "SIM(%x) destroyed in thread %s", inst, QThread::currentThreadId());
    }
}

void SIM::onEventFromQML(QQmlApplicationEngine *engine, QString name, QByteArray data)
{
    onEventReceivedRaw_in in;
    onEventReceivedRaw_out out;
    auto handle_v = engine->property("simHandle");
    if(!handle_v.isValid())
    {
        sim::addLog(sim_verbosity_errors, "SIM::onEventFromQML: internal error: engine doesn't have a valid handle");
        return;
    }
    in.engineHandle = handle_v.toString().toStdString();
    in.eventName = name.toStdString();
    in.eventData = std::string(data.constData(), data.length());

    sim::addLog(sim_verbosity_debug, "SIM::onEventFromQML(%s, %s)", in.eventName, in.eventData);

    auto scriptID_v = engine->property("eventHandlerScriptID");
    auto functionName_v = engine->property("eventHandlerFunctionName");
    if(!scriptID_v.isValid() || !functionName_v.isValid())
    {
        sim::addLog(sim_verbosity_errors, "SIM::onEventFromQML: engine doesn't have a valid event handler (forgot to call simQML.setEventHandler?)");
        return;
    }

    int scriptID = scriptID_v.toInt();
    std::string functionName = functionName_v.toString().toStdString();
    sim::addLog(sim_verbosity_debug, "SIM::onEventFromQML: calling handler scriptID=%d functionName=%s", scriptID, functionName);
    onEventReceivedRaw(scriptID, functionName.c_str(), &in, &out);
}

#ifdef Qt5_Quick3D_FOUND
void SIM::onGetMeshData(int shapeHandle, Geometry *geom)
{
    simFloat *verticesBuf;
    simInt verticesSize;
    simInt *indicesBuf;
    simInt indicesSize;
    if(-1 != simGetShapeMesh(shapeHandle, &verticesBuf, &verticesSize, &indicesBuf, &indicesSize, nullptr))
    {
        QByteArray vertexData((const char*)verticesBuf, verticesSize * sizeof(float));
        simReleaseBuffer(reinterpret_cast<const char*>(verticesBuf));

        QByteArray indexData((const char*)indicesBuf, indicesSize * sizeof(int));
        simReleaseBuffer(reinterpret_cast<const char*>(indicesBuf));

        emit updateMeshData(geom, vertexData, indexData);
    }
}
#endif // Qt5_Quick3D_FOUND
