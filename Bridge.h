#ifndef BRIDGE_H_INCLUDED
#define BRIDGE_H_INCLUDED

#include "config.h"

#include <QObject>
#include <QString>
#include <QWidget>
#include <QQmlApplicationEngine>

#include "stubs.h"

class Bridge : public QObject
{
    Q_OBJECT

public:
    static void registerQmlType();

    Bridge(QObject *parent = 0);
    virtual ~Bridge();

    Q_PROPERTY(int platform READ platform);

    int platform() const;

    Q_INVOKABLE void sendEventRaw(QString name, QByteArray data);

signals:
    void eventFromQML(QQmlApplicationEngine *engine, QString name, QByteArray data);
    void eventReceived(QString name, QByteArray data);

public slots:
    void eventFromSIM(QQmlApplicationEngine *engine, QString name, QByteArray data);

public:
    static int platform_;
};

#endif // BRIDGE_H_INCLUDED

