#include "Bridge.h"
#include "SIM.h"

#include <simPlusPlus/Lib.h>

int Bridge::platform_ = -1;

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
}

Bridge::~Bridge()
{
}

void Bridge::eventFromSIM(QQmlApplicationEngine *engine, QString name, QByteArray data)
{
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

int Bridge::platform() const
{
    return platform_;
}

void Bridge::sendEventRaw(QString name, QByteArray data)
{
    sim::addLog(sim_verbosity_debug, "Bridge::sendEventRaw(\"%s\", \"%s\")", name.toStdString(), std::string(data.constData(), data.length()));
    auto engine = static_cast<QQmlApplicationEngine *>(qmlEngine(this));
    emit eventFromQML(engine, name, data);
}
