#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>

class Image : public QImage
{
public:
    Image();
    Image(const QString &fileName);
    Image(const QImage &);
    Image(const QSize &size, Format format);
    Image(int width, int height, Format format);
    Image(uchar *data, int width, int height, Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr);
    Image(const uchar *data, int width, int height, Format format, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr);

    void line(int x0, int y0, int x1, int y1, QColor color = QColor(255, 0, 0));
};

#endif // IMAGE_H
