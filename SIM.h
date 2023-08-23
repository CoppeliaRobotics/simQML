#ifndef SIM_H_INCLUDED
#define SIM_H_INCLUDED

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QQmlApplicationEngine>

#include "Geometry.h"

class SIM : public QObject
{
    Q_OBJECT

public:
    virtual ~SIM();

    static SIM * getInstance(QObject *parent = nullptr);
    static void destroyInstance();

private:
    SIM(QObject *parent = nullptr);

    static SIM *instance;

signals:
    void createEngine(QQmlApplicationEngine **engine, const QStringList &importPaths);
    void destroyEngine(QQmlApplicationEngine *engine);
    void setEventHandler(QQmlApplicationEngine *engine, int scriptID, QString functionName);
    void setEngineHandle(QQmlApplicationEngine *engine, QString handle);
    void load(QQmlApplicationEngine *engine, QString filename, QString contextInfo);
    void loadData(QQmlApplicationEngine *engine, QByteArray data, QString basepath, QString contextInfo);
    void sendEventToQML(QQmlApplicationEngine *engine, QString name, QByteArray data);

public slots:
    void onEventFromQML(QQmlApplicationEngine *engine, QString name, QByteArray data);

public slots:
    void onGetMeshData(int shapeHandle, Geometry *geom);

signals:
    void updateMeshData(Geometry *geom, QByteArray vertexData, QByteArray indexData);
};

#endif // SIM_H_INCLUDED

