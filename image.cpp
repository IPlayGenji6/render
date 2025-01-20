#include "image.h"
#include <QFile>
#include <QDebug>

Image::Image() {}

Image::Image(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray binData = file.readAll();
    *this = QImage::fromData(binData);
}

Image::Image(const QImage &rhs)
    : QImage(rhs)
{}

Image::Image(const QSize &size, Format format)
    : QImage(size, format)
{}

Image::Image(int width, int height, Format format)
    : QImage(width, height, format)
{}

Image::Image(uchar *data, int width, int height, Format format, QImageCleanupFunction cleanupFunction, void *cleanupInfo)
    : QImage(data, width, height, format, cleanupFunction, cleanupInfo)
{}

Image::Image(const uchar *data, int width, int height, Format format, QImageCleanupFunction cleanupFunction, void *cleanupInfo)
    : QImage(data, width, height, format, cleanupFunction, cleanupInfo)
{}

void Image::line(int x0, int y0, int x1, int y1, QColor color)
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    // http://members.chello.at/~easyfilter/Bresenham.pdf (page12)
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int exy = dx + dy;
    int e2;

    while (true) {
        this->setPixelColor(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * exy;
        if (e2 >= dy) {
            exy = exy + dy; // Δe = ex = dy
            x0 = x0 + sx;
        }
        if (e2 <= dx) {
            exy = exy + dx;
            y0 = y0 + sy;
        }
    }
}


