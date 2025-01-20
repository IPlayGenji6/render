#include "model.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDebug>

Model::Model(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading:" << file.errorString();
        return;
    }
    QTextStream in(&file);
    QString line;

    while (!in.atEnd()) {
        line = in.readLine();
        QTextStream lineStream(&line);
        QString type;
        lineStream >> type;

        if (type == "v") {
            float x, y, z;
            lineStream >> x >> y >> z;
            m_verts.push_back(QVector3D(x, y, z));
        } else if (type == "vt") {
            float u, v;
            lineStream >> u >> v;
            m_textureCoord.push_back(QVector2D(u, 1 - v)); // obj文件vt坐标原点位于左下角，此处变换到左上角
        } else if (type == "vn") {
            float x, y, z;
            lineStream >> x >> y >> z;
            m_normals.push_back(QVector3D(x, y ,z));
        } else if (type == "f") {
            int v, t, n;
            char slash; // 吸收'/'
            for (int i = 0; i < 3; i++) {                
                lineStream >> v >> slash >> t >> slash >> n; 
                m_faceVertIndex.push_back(v - 1); // obj文件索引从1开始
                m_faceTextIndex.push_back(t - 1);
                m_faceNormIndex.push_back(n - 1);
            }
        }
    }

    file.close();

    QFileInfo modelFileInfo(fileName);
    m_modelName = modelFileInfo.baseName();
    QString modelPath = modelFileInfo.absolutePath();
    QDir modelDirectory(modelPath);
    QStringList filters;
    QStringList imageExtensions = {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff"};
    QStringList keywords = {"diff", "DIFF", "norm", "NORM", "spec", "SPEC"};

    for (const QString &keyword : keywords) {
        for (const QString &extension : imageExtensions) {
            filters << m_modelName + "*" + keyword + "*" + extension;
        }
    }
    modelDirectory.setNameFilters(filters);
    QStringList files = modelDirectory.entryList();

    if (!files.isEmpty()) {
        for (const QString &file : files) {
            QString fullPath = modelPath + "/" + file;
            loadTexture(fullPath);
        }
    } else {
        qDebug() << "No texture files found that match the criteria.";
    }

}

int Model::vertsSize() const
{
    return m_verts.size();
}

int Model::facesSize() const
{
    return m_faceVertIndex.size() / 3;
}

QVector3D Model::vert(const int index) const
{
    if (index < vertsSize())
        return QVector3D(0,0,0);

    return m_verts[index];
}

/**
 * @brief 0 <= vertIndex <= 2
 */
QVector3D Model::vert(const int faceIndex, const int vertIndex) const
{
    return m_verts[m_faceVertIndex[faceIndex * 3 + vertIndex]];
}

QVector3D Model::normal(const int faceIndex, const int vertIndex) const
{
    return m_normals[m_faceNormIndex[faceIndex * 3 + vertIndex]];
}

QVector2D Model::texture(const int faceIndex, const int vertIndex) const
{
    return m_textureCoord[m_faceTextIndex[faceIndex * 3 + vertIndex]];
}

void Model::loadTexture(QString fileName)
{
    QFileInfo fileInfo(fileName);
    if (!fileInfo.isFile())
        return;

    QString baseName = fileInfo.baseName();
    if(baseName.lastIndexOf("diff", Qt::CaseInsensitive) >= 0) {
        m_diffuseMap.load(fileName);
        qDebug() << "Model" << m_modelName+".obj" << "has loaded diffuse map" << fileInfo.fileName();
    } else if (baseName.lastIndexOf("norm", Qt::CaseInsensitive) >= 0) {
        m_normalMap.load(fileName);
        qDebug() << "Model" << m_modelName+".obj" << "has loaded normal map" << fileInfo.fileName();
    } else if (baseName.lastIndexOf("spec", Qt::CaseInsensitive) >= 0) {
        m_specularMap.load((fileName));
        qDebug() << "Model" << m_modelName+".obj" << "has loaded specular map" << fileInfo.fileName();
    }
}



