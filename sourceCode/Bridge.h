#ifndef BRIDGE_H_INCLUDED
#define BRIDGE_H_INCLUDED

#include "config.h"

#include <QObject>
#include <QString>
#include <QWidget>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <simPlusPlus-2/Lib.h>

#include "stubs.h"

class Bridge : public QObject
{
    Q_OBJECT

public:
    static void registerQmlType();

    Bridge(QObject *parent = 0);
    virtual ~Bridge();

    bool eventFilter(QObject *object, QEvent *event);

    Q_INVOKABLE void sendEventRaw(QString name, QByteArray data);

    Q_INVOKABLE void raiseAboveMainWindow(QQuickWindow *window);

signals:
    void eventFromQML(QQmlApplicationEngine *engine, QString name, QByteArray data);
    void eventReceived(QString name, QByteArray data);

public slots:
    void eventFromSIM(QQmlApplicationEngine *engine, QString name, QByteArray data);
};

#endif // BRIDGE_H_INCLUDED

