#include "ImageItem.h"

ImageItem::ImageItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    this->m_currentImage = QImage(":/nomedia1.png");
}

void ImageItem::paint(QPainter *painter)
{
    QRectF bounding_rect = boundingRect();
    QImage scaled = this->m_currentImage.scaledToHeight(bounding_rect.height());
    QPointF center = bounding_rect.center() - scaled.rect().center();

    if(center.x() < 0)
        center.setX(0);
    if(center.y() < 0)
        center.setY(0);
    painter->drawImage(center, scaled);
}

QImage ImageItem::image() const
{
    return this->m_currentImage;
}

void ImageItem::setImage(const QImage &image)
{
    this->m_currentImage = image;
    update();
}
