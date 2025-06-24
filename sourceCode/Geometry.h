#ifndef COPPELIASIMGEOMETRY_H_INCLUDED
#define COPPELIASIMGEOMETRY_H_INCLUDED

#include <QQuick3DGeometry>
#include <simPlusPlus/Lib.h>

class Geometry : public QQuick3DGeometry
{
    Q_OBJECT
    Q_PROPERTY(int shapeHandle READ shapeHandle WRITE setShapeHandle NOTIFY shapeHandleChanged)

public:
    static void registerQmlType();

    Geometry();
    virtual ~Geometry();

    void setShapeHandle(const int &a);
    int shapeHandle() const;

    void setMeshData(QByteArray vertexData, QByteArray indexData);

signals:
    void shapeHandleChanged();

private:
    void rebuildGeometry();

    int shapeHandle_;
};

#endif // COPPELIASIMGEOMETRY_H_INCLUDED
