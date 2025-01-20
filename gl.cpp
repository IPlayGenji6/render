#include "gl.h"
#include <QDebug>
#include <iostream>

Eigen::Matrix4f modelMat = Eigen::Matrix4f::Identity();
Eigen::Matrix4f viewMat = Eigen::Matrix4f::Identity();
Eigen::Matrix4f projectionMat = Eigen::Matrix4f::Identity();
Eigen::Matrix4f viewportMat = Eigen::Matrix4f::Identity();

void lookAt(const Eigen::Vector3f eye, const Eigen::Vector3f center, const Eigen::Vector3f up)
{
    Eigen::Vector3f ass = (eye - center).normalized();
    Eigen::Vector3f right = up.cross(ass).normalized();
    Eigen::Vector3f _up = ass.cross(right).normalized();

    viewMat(0, 0) = right.x();
    viewMat(0, 1) = right.y();
    viewMat(0, 2) = right.z();
    viewMat(0, 3) = -right.dot(eye);
    viewMat(1, 0) = _up.x();
    viewMat(1, 1) = _up.y();
    viewMat(1, 2) = _up.z();
    viewMat(1, 3) = -_up.dot(eye);
    viewMat(2, 0) = ass.x();
    viewMat(2, 1) = ass.y();
    viewMat(2, 2) = ass.z();
    viewMat(2, 3) = -ass.dot(eye);
}

void projection(float coeff)
{
    projectionMat <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, -1/coeff, 1;
}

void viewport(int x, int y, int w, int h)
{
    // d是z 缓冲区的分辨率
    // int d = 255;
    // viewportMat <<
    //     w/2.0f, 0, 0, w/2.0f + x,
    //     0, h/2.0f, 0, h/2.0f + y,
    //     0, 0, d/2.0f, d/2.0f,
    //     0, 0, 0, 1;
    viewportMat <<
        w/2.0f, 0, 0, w/2.0f + x,
        0, h/2.0f, 0, h/2.0f + y,
        0, 0, 1, 0,
        0, 0, 0, 1;
}

Shader::Shader(const Model &model)
    : model_(model)
{
    uniform_lightDir = Eigen::Vector3f(1, 2, 1);
    uniform_lightDir.normalize();
}

QColor Shader::sample2D(const QImage &img, Eigen::Vector2f &uv)
{
    if (uv.x() < 0 || uv.y() < 0) {
        return QColor(255,255,255);
    }
    QColor c = img.pixelColor(uv.x() * img.width(), uv.y() * img.height());;
    return c;
}

/**
 * @brief 顶点着色器
 * @param faceIndex
 * @param vertIndex
 * @param gl_Position
 */
void Shader::vertex(const int faceIndex, const int vertIndex, Eigen::Vector4f &gl_Position)
{
    // texture
    QVector2D vertUV = model_.texture(faceIndex, vertIndex);
    triangle_uv_(0, vertIndex) = vertUV.x();
    triangle_uv_(1, vertIndex) = vertUV.y();

    // normal (https://github.com/ssloy/tinyrenderer/wiki/Lesson-5:-Moving-the-camera#transformation-of-normal-vectors)
    QVector3D vertNORM = model_.normal(faceIndex, vertIndex);
    Eigen::Matrix4f invTransform = (viewMat * modelMat).inverse();
    Eigen::Vector4f trans_normal = invTransform * Eigen::Vector4f(vertNORM.x(), vertNORM.y(), vertNORM.z(), 0.0f);
    vertex_normals_(0, vertIndex) = trans_normal.x();
    vertex_normals_(1, vertIndex) = trans_normal.y();
    vertex_normals_(2, vertIndex) = trans_normal.z();

    QVector3D v = model_.vert(faceIndex, vertIndex);
    gl_Position = viewMat * modelMat * Eigen::Vector4f(v.x(), v.y(), v.z(), 1.0f);
    gl_Position = projectionMat * gl_Position;
}

/**
 * @brief 片段着色器
 * @param barycenter
 * @param fragColor
 */
void Shader::fragment(const Eigen::Vector3f barycenter, QColor &fragColor)
{
    // 顶点纹理插值
    Eigen::Vector2f uv = triangle_uv_ * barycenter;

    // QColor color = sample2D(model.m_diffuseMap, uv);

    // 顶点法线插值
    Eigen::Vector3f bn = (vertex_normals_ * barycenter).normalized();

    float brightness = bn.dot(uniform_lightDir);
    brightness = brightness > 0 ? brightness : 0;
    QColor color = QColor(255*brightness, 255*brightness, 255*brightness);
    fragColor = color;
}

/**
 * @brief 三角面绘制
 * @param v0
 * @param v1
 * @param v2
 * @param image
 * @param zBuffer
 */
void Shader::triangle(const Eigen::Vector4f clipVerts[3], QImage &image, QVector<QVector<float> > &zBuffer)
{
    Eigen::Vector4f screenCoords[3] = {viewportMat * clipVerts[0], viewportMat * clipVerts[1], viewportMat * clipVerts[2]};
    QPoint t0 = QPoint(screenCoords[0].x(), screenCoords[0].y());
    QPoint t1 = QPoint(screenCoords[1].x(), screenCoords[1].y());
    QPoint t2 = QPoint(screenCoords[2].x(), screenCoords[2].y());
    QPoint t00 = t0;
    QPoint t11 = t1;
    QPoint t22 = t2;

    if (t0.y() > t1.y())
        std::swap(t0, t1);
    if (t1.y() > t2.y())
        std::swap(t1, t2);
    if (t0.y() > t1.y())
        std::swap(t0, t1);

    int totalHeight = t2.y() - t0.y();
    bool isUpperTriangle = false;
    for (int y = 0; y < totalHeight; ++y) {
        isUpperTriangle = y > t1.y() - t0.y() || t1.y() == t0.y();
        int currHalfHeight = isUpperTriangle ? t2.y() - t1.y() : t1.y() - t0.y();
        float kTotal = static_cast<float>(y) / totalHeight;
        float kCurrHalf = (static_cast<float>(y) - (isUpperTriangle ? t1.y() - t0.y() : 0)) / currHalfHeight;
        QPoint A = t0 + (t2-t0) * kTotal;
        QPoint B = isUpperTriangle ? t1 + (t2-t1) * kCurrHalf : t0 + (t1-t0) * kCurrHalf;
        if (A.x() > B.x()) std::swap(A, B);
        float zA = screenCoords[0].z() + (screenCoords[2].z() - screenCoords[0].z())*kTotal;
        float zB = isUpperTriangle ? screenCoords[1].z() + (screenCoords[2].z() - screenCoords[1].z())*kCurrHalf : screenCoords[0].z() + (screenCoords[1].z() - screenCoords[0].z())*kCurrHalf;
        for (int x = A.x(); x <= B.x(); ++x) {
            float kHorizon = A.x() == B.x() ? 0 : static_cast<float>(x - A.x()) / (B.x() - A.x());
            float z = A.x() == B.x() ? 0 : zA + (zB - zA)*kHorizon;
            if (z > zBuffer[x][t0.y() + y]) {
                // texture
                Eigen::Vector2f tri[3];
                tri[0] = Eigen::Vector2f(t00.x(), t00.y());
                tri[1] = Eigen::Vector2f(t11.x(), t11.y());
                tri[2] = Eigen::Vector2f(t22.x(), t22.y());
                Eigen::Vector3f bc = barycentric(tri, Eigen::Vector2f(x, t0.y() + y));
                QColor color;
                this->fragment(bc, color);
                image.setPixelColor(x, t0.y() + y, color);
                zBuffer[x][t0.y() + y] = z;
            }
        }
    }
}

/**
 * @brief 三角形重心坐标
 * @param tri
 * @param P
 * @see https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Vertex_approach
 * @return
 */
Eigen::Vector3f barycentric(const Eigen::Vector2f tri[3], const Eigen::Vector2f P)
{
    Eigen::Matrix3f T;
    for (int i = 0; i < 3; ++i) {
        T(0, i) = tri[i].x();
        T(1, i) = tri[i].y();
        T(2, i) = 1.0f;
    }
    // 退化三角形
    // if (T.determinant() < 1e-6) {
    //     return Eigen::Vector3f(-1, -1, -1);
    // }
    Eigen::Vector3f P_1(P(0), P(1), 1.0f);
    Eigen::Vector3f lambda = T.inverse() * P_1;
    // std::cout << "A:" << tri[0] << "\nB:" << tri[1] << "\nC:" << tri[2] << "\nP:" << P << "\n" << lambda << std::endl << std::endl;
    return lambda;
}
