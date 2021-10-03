#include "header.h"

QImage MergeImage(const QImage& baseImage, const QImage& overlayImage)  //合并两张图片
{
    QImage overlay = overlayImage.scaled(baseImage.size());
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, overlay);

    painter.end();

    return imageWithOverlay;
}

QPixmap RotatePixmap(const QPixmap &pixmap, QLabel *target, QLabel *src)
{
    double dx=target->x()-src->x(), dy=src->y()-target->y();
    double dis = DISTANCE(dx, dy);
    double Sin= dx/dis, Cos=dy/dis;
    QMatrix mat(Cos, Sin, -Sin, Cos, 0, 0);
    return pixmap.transformed(mat);
}
