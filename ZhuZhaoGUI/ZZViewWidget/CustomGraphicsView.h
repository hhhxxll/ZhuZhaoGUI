#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView> // 引入视图视口义
#include <QWidget>       // 引入基础窗口部件
#include <QEvent>        // 引入底层硬件事件（鼠标、键盘操作）的数据结构
#include <QLabel>        // 引入文本标签控件

class CustomImageItem;
class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    CustomGraphicsView(QWidget *parent = 0);
    ~CustomGraphicsView();
    bool InitWidget();
    void SetImage(const QImage & qImage);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

public slots:
    void onCenter();
    void onZoom(float fScaleFactor);

private:
    void fitFrame();
    void setBackground(bool enabled = true,bool invertColor = false);

private:
    double m_dZoomValue = 1;

    QGraphicsScene* m_pScene;
    CustomImageItem* m_pImageItem;
    QWidget* m_pPosInfoWidget;
    QLabel* m_pPosInfoLabel;

    QPixmap m_Image;
    QImage m_qImage;
    QPixmap m_tilePixmap = QPixmap(36,36);


};

#endif // CUSTOMGRAPHICSVIEW_H
