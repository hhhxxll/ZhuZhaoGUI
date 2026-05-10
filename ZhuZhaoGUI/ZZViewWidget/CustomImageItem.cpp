#include "CustomImageItem.h"
#include <QGraphicsSceneHoverEvent>     // 引入鼠标悬浮事件处理的工具库

CustomImageItem::CustomImageItem(QWidget *parent)
    : QGraphicsPixmapItem(nullptr)
{
    this->setAcceptHoverEvents(true);
}

void CustomImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF mousePosition = event->pos();//从事件中拿到鼠标当前图片上的具体位置
    int R, G, B;
    int x, y;
    x = mousePosition.x();
    y = mousePosition.y();
    QImage img = pixmap().toImage();
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= img.width())  x = img.width() - 1;
    if (y >= img.height()) y = img.height() - 1;

    img.pixelColor(x, y).getRgb(&R, &G, &B);
    QString InfoVal = QString(" W:%1,H:%2 | X:%3,Y:%4 | R:%5,G:%6,B:%7")
                          .arg(QString::number(w),
                               QString::number(h),
                               QString::number(x),
                               QString::number(y),
                               QString::number(R),
                               QString::number(G),
                               QString::number(B));

    emit RGBValue(InfoVal);
}
