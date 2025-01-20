#ifndef GL_H
#define GL_H

#include "model.h"
#include "Eigen/Dense"

/**
     * @see https://blog.songjiahao.com/archives/993
     */
void lookAt(const Eigen::Vector3f eye, const Eigen::Vector3f center, const Eigen::Vector3f up);

/**
     * @see https://github.com/ssloy/tinyrenderer/wiki/Lesson-4:-Perspective-projection
     */
void projection(float coeff);

/**
     * @brief 双单位立方体 [-1,1]*[-1,1]*[-1,1] 被映射到屏幕立方体 [x,x+w]*[y,y+h]*[0,d] 上
     */
void viewport(int x, int y, int w, int h);

class Shader
{
public:
    Shader(const Model &model);

    static QColor sample2D(const QImage &img, Eigen::Vector2f &uv);

    void vertex(const int faceIndex, const int vertIndex, Eigen::Vector4f &gl_Position);
    void fragment(const Eigen::Vector3f barycenter, QColor &fragColor);
    void triangle(const Eigen::Vector4f clipVerts[3], QImage &image, QVector<QVector<float>> &zBuffer);

private:
    const Model model_;
    // int WIDTH;
    // int HEIGHT;
    Eigen::Matrix<float, 2, 3> triangle_uv_;
    Eigen::Matrix<float, 3, 3> vertex_normals_;

    Eigen::Vector3f uniform_lightPosition;
    Eigen::Vector3f uniform_lightDir;
    Eigen::Vector3f uniform_lightColor;
};

Eigen::Vector3f barycentric(const Eigen::Vector2f tri[3], const Eigen::Vector2f P);

#endif // GL_H
