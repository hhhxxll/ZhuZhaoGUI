#include "LitImgItemWidget.h"
#include <QLayout>
#include <QPainter>
#include <QPixmap>


LitImgItemWidget::LitImgItemWidget(QWidget *parent)
    : QWidget{parent}
{
    this ->setFocusPolicy(Qt::StrongFocus);
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

LitImgItemWidget::~LitImgItemWidget()
{

}

bool LitImgItemWidget::InitWidget()
{
    return true;
}

void LitImgItemWidget::setImage(QImage& qImage)
{
    m_qImg = qImage;
}

void LitImgItemWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int nDevceWidth = this->width();
    int nDeviceHeight = this->height();

    if(m_qImg.isNull())
    {
        m_qImg = QImage("://Resouce/icon/nullImg.png");
    }
    QPixmap tempPixmap = QPixmap::fromImage(m_qImg);
    painter.drawPixmap(2, 2, nDevceWidth-4, nDeviceHeight-4, tempPixmap);

    QPen pen;
    pen.setColor(QColor(this->hasFocus() ? Qt::red : Qt::black));
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawRect(QRect(2,2,nDevceWidth-4,nDeviceHeight-4));
}

