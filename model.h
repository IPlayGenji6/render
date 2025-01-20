#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QVector3D>
#include "image.h"

class Model
{
public:
    Model(QString fileName);

    int vertsSize() const;
    int facesSize() const;
    QVector3D vert(const int index) const;
    QVector3D vert(const int faceIndex, const int vertIndex) const;
    QVector3D normal(const int faceIndex, const int vertIndex) const;
    QVector2D texture(const int faceIndex, const int vertIndex) const;

    void loadTexture(QString fileName);

    QString m_modelName;

    QVector<QVector3D> m_verts;
    QVector<QVector2D> m_textureCoord;
    QVector<QVector3D> m_normals;

    QVector<int> m_faceVertIndex;
    QVector<int> m_faceTextIndex;
    QVector<int> m_faceNormIndex;

    QImage m_diffuseMap;
    QImage m_normalMap;
    QImage m_specularMap;

};

#endif // MODEL_H
