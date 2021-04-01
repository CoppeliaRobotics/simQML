#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <QObject>
#include <QString>
#include <QWidget>
#include <QQmlApplicationEngine>

#include "Geometry.h"

class UI : public QObject
{
    Q_OBJECT

public:
    virtual ~UI();

    static UI * getInstance(QObject *parent = nullptr);
    static void destroyInstance();

private:
    UI(QObject *parent = nullptr);

    static UI *instance;

public:
    static QWidget *simMainWindow;
    static float wheelZoomFactor;

signals:
    void getMeshData(int shapeHandle, Geometry *geom);

public slots:
    void onCreateEngine(QQmlApplicationEngine **engine, const QStringList &importPaths);
    void onDestroyEngine(QQmlApplicationEngine *engine);
    void onSetEventHandler(QQmlApplicationEngine *engine, int scriptID, QString functionName);
    void onSetEngineHandle(QQmlApplicationEngine *engine, QString handle);
    void onLoad(QQmlApplicationEngine *engine, QString filename, QString contextInfo);
    void onLoadData(QQmlApplicationEngine *engine, QByteArray data, QString basepath, QString contextInfo);
    void onSetMeshData(Geometry *geom, QByteArray vertexData, QByteArray indexData);
};

#endif // UI_H_INCLUDED

