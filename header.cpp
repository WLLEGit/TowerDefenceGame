#include "header.h"

Cell::Cell(int type, int resourceType)
    :resourceType(resourceType)
{
    SetType(type);
}

void Cell::SetType(int type)
{
    this->type = type;
    if(type & Path)
        cellType = Path;
    else if(type & Blocked)
        cellType = Blocked;
    else if(type & Placable)
        cellType = Placable;
}

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
