#include "SIM.h"
#include "UI.h"
#include "stubs.h"

#include <QThread>

// SIM is a singleton

SIM *SIM::instance = nullptr;

SIM::SIM(QObject *parent)
    : QObject(parent)
{
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
        delete SIM::instance; //SIM::instance->deleteLater(); crashes on quit
        SIM::instance = nullptr;
        sim::addLog(sim_verbosity_debug, "SIM(%x) destroyed in thread %s", inst, QThread::currentThreadId());
    }
}

void SIM::onEventFromQML(QQmlApplicationEngine *engine, QString name, QByteArray data)
{
    if(!isEngineInstanceValid(engine))
    {
        sim::addLog(sim_verbosity_warnings, "SIM::onEventFromQML: received event from an engine that is already destroyed");
        return;
    }
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

void SIM::onGetMeshData(int shapeHandle, Geometry *geom)
{
    double *verticesBuf;
    int verticesSize;
    int *indicesBuf;
    int indicesSize;
    try
    {
        sim::getShapeMesh(shapeHandle, &verticesBuf, &verticesSize, &indicesBuf, &indicesSize, nullptr);

        QByteArray vertexData((const char*)verticesBuf, verticesSize * sizeof(float));
        sim::releaseBuffer(reinterpret_cast<const char*>(verticesBuf));

        QByteArray indexData((const char*)indicesBuf, indicesSize * sizeof(int));
        sim::releaseBuffer(reinterpret_cast<const char*>(indicesBuf));

        emit updateMeshData(geom, vertexData, indexData);
    }
    catch(...) {}
}

void SIM::addEngineInstance(void *inst)
{
    QMutexLocker locker(&engineInstancesMutex);
    engineInstances.insert(inst);
}

void SIM::removeEngineInstance(void *inst)
{
    QMutexLocker locker(&engineInstancesMutex);
    engineInstances.remove(inst);
}

bool SIM::isEngineInstanceValid(void *inst)
{
    QMutexLocker locker(&engineInstancesMutex);
    return engineInstances.contains(inst);
}
