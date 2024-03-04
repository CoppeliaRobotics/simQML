#include "Geometry.h"
#include "UI.h"

#include <QVector3D>

#include <simPlusPlus/Lib.h>

void Geometry::registerQmlType()
{
    qmlRegisterType<Geometry>("CoppeliaSimPlugin.Geometry", 1, 0, "CoppeliaSimGeometry");
}

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

void Geometry::setShapeHandle(const int &a)
{
    if(a != shapeHandle_)
    {
        shapeHandle_ = a;
        UI::getInstance()->getMeshData(shapeHandle_, this);
        emit shapeHandleChanged();
    }
}

int Geometry::shapeHandle() const
{
    return shapeHandle_;
}

void Geometry::setMeshData(QByteArray vertexData, QByteArray indexData)
{
    clear();
    setPrimitiveType(PrimitiveType::Triangles);
    addAttribute(Attribute::PositionSemantic, 0, Attribute::F32Type);
    addAttribute(Attribute::IndexSemantic, 0, Attribute::U32Type);
    addAttribute(Attribute::NormalSemantic, 0, Attribute::F32Type);
    setVertexData(vertexData);
    setIndexData(indexData);
    setStride(sizeof(float) * 3);
    QVector3D boundsMin, boundsMax;
    auto vertices = reinterpret_cast<const float*>(vertexData.constData());
    auto indices = reinterpret_cast<const int*>(indexData.constData());
    for(size_t i = 0; i < vertexData.length() / sizeof(float); i += 3)
    {
        boundsMin.setX(std::min(vertices[i + 0], i == 0 ? vertices[i + 0] : boundsMin.x()));
        boundsMin.setY(std::min(vertices[i + 1], i == 0 ? vertices[i + 1] : boundsMin.y()));
        boundsMin.setZ(std::min(vertices[i + 2], i == 0 ? vertices[i + 2] : boundsMin.z()));
        boundsMax.setX(std::max(vertices[i + 0], i == 0 ? vertices[i + 0] : boundsMax.x()));
        boundsMax.setY(std::max(vertices[i + 1], i == 0 ? vertices[i + 1] : boundsMax.y()));
        boundsMax.setZ(std::max(vertices[i + 2], i == 0 ? vertices[i + 2] : boundsMax.z()));
    }
    setBounds(boundsMin, boundsMax);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setName(QStringLiteral("CoppeliaSimGeometry-%1").arg(shapeHandle_));
#endif
}
