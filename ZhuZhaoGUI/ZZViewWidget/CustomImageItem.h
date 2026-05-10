#ifndef CUSTOMIMAGEITEM_H
#define CUSTOMIMAGEITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

class CustomImageItem :public QObject,public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    CustomImageItem(QWidget *parent = nullptr);

protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

public:
    int w;
    int h;

signals:
    void RGBValue(QString InfoVal);
};

#endif // CUSTOMIMAGEITEM_H
