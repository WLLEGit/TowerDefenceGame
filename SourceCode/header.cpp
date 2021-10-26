#include "header.h"

QJsonObject globalConfig;
int CELLWIDTH;
int FPS;
int MAPID;
int ROUNDID;

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

void ReadConfigFromJsonFile()
{
    QFile file(PROJECTPATH+ "assets/config.json");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Config Load Failed";
        assert(false);
    }
    QJsonParseError *error=new QJsonParseError;
    QJsonDocument jdc=QJsonDocument::fromJson(file.readAll(),error);
    if(error->error!=QJsonParseError::NoError)
    {
        qDebug()<<"parseJson:"<<error->errorString();
        assert(false);
    }

    globalConfig = jdc.object();
    FPS = globalConfig["FPS"].toInt();
    CELLWIDTH = globalConfig["CELLWIDTH"].toInt();
    MAPID = globalConfig["MAPID"].toInt();
    ROUNDID = globalConfig["ROUNDID"].toInt();
}
