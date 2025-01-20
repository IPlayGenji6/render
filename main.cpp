#include "mainwindow.h"
#include <QApplication>
#include "Eigen/Dense"
#include "image.h"
#include "model.h"
#include "gl.h"


const int WIDTH = 800;
const int HEIGHT =800;

Eigen::Vector3f eye(0, 0, 3);
Eigen::Vector3f center(0, 0, 0);
Eigen::Vector3f up(0, 1, 0);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString directory("C:/Users/71058/Downloads/lp_blinn8SG/");
    Model model(directory + "lp_blinn8SG.obj");

    QVector3D lightDir(0, 0, 1);
    lightDir.normalize();

    Image img(WIDTH, HEIGHT, QImage::Format_ARGB32);
    img.fill(QColor(0, 0, 0));

    Shader shader(model);

    lookAt(eye, center, up);
    viewport(0, 0, WIDTH, HEIGHT);
    projection((eye-center).norm());

    // z-buffer
    QVector<QVector<float>> zBuffer(WIDTH, QVector<float>(HEIGHT, std::numeric_limits<float>::lowest()));

    for (int face = 0; face < model.facesSize(); ++face) {
        Eigen::Vector4f clipVert[3];
        for (int i : {0, 1, 2}) {
            shader.vertex(face, i, clipVert[i]);
        }

        shader.triangle(clipVert, img, zBuffer);
    }
    img.mirror();
    if (img.save(directory + "res.png"))
        qDebug() << "image saved successfully.";

    MainWindow w;
    w.show();
    w.dispImg(img);
    return a.exec();

    return 0;
}
