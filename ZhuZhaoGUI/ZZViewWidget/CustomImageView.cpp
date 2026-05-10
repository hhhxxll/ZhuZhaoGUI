#include "CustomImageView.h"
#include <QLayout>
#include "CustomGraphicsView.h"

CustomImageView::CustomImageView(QWidget *parent)
    : QWidget{parent}
    , m_pGraphicsview(Q_NULLPTR)
{
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

CustomImageView::~CustomImageView()
{

}

bool CustomImageView::InitWidget()
{
    m_pGraphicsview = new CustomGraphicsView(this);

    QHBoxLayout* pViewLayout = new QHBoxLayout();

    pViewLayout->setSpacing(0);
    pViewLayout->setContentsMargins(0,0,0,0);
    pViewLayout->addWidget(m_pGraphicsview);
    this->setLayout(pViewLayout);

    return true;
}

//显示算法处理后的图片
void CustomImageView::SetImage(QImage &qImage)
{
    m_qImage = qImage;
    m_pGraphicsview->SetImage(qImage);
}

//显示原图
void CustomImageView::SetImage(QString &strPath)
{
    QImage img = QImage(strPath);
    if(!img.isNull())
    {
        m_qImage = img;
        m_pGraphicsview->SetImage(img);
    }
}

void CustomImageView::OnSendImage(QImage & qImage)
{
    SetImage(qImage);
}