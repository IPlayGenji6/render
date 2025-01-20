#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include "image.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void dispImg(const QImage &img);

private:
    Ui::MainWindow *ui;
};

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    RenderWidget(QWidget *parent = nullptr) : QWidget(parent) {};

    void setImage(const Image &image) {
        image_ = image;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);

        if (!image_.isNull()) {
            painter.drawImage(0, 0, image_);
        }
    }

private:
    Image image_;
};

#endif // MAINWINDOW_H
