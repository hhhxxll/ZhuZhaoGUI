#include "ZZOneParamWidget.h"
#include <QLayout>
#include <QMessageBox>
#include <QFileDialog>
#include "ZZListener.h"

#define OneParamWidgetHeiht (28)

ZZOneParamWidget::ZZOneParamWidget(QString paramName, QWidget *parent)
    : QWidget{parent}
    , m_strParamName(paramName)
    , m_pTitleLabel(Q_NULLPTR)
    , m_pSlantsLabel(Q_NULLPTR)
    , m_pSlantsSpin(Q_NULLPTR)
    , m_pTiltsLabel(Q_NULLPTR)
    , m_pTiltsSpin(Q_NULLPTR)
    , m_pLoadImageBtn(Q_NULLPTR)
{
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

bool ZZOneParamWidget::InitWidget()
{
    //创建参数标题标签
    QFont font("Microsoft YaHei", 8);
    font.setBold(true);

    m_pTitleLabel = new QLabel(this);
    m_pTitleLabel->setText(m_strParamName);
    m_pTitleLabel->setFont(font);

    // 创建显示文字“Slant”的标签
    m_pSlantsLabel = new QLabel(this);
    m_pSlantsLabel->setText(tr("Slant"));
    m_pSlantsLabel->setFixedHeight(OneParamWidgetHeiht);

    // 创建输入 Slant 数值的小数数字框
    m_pSlantsSpin = new  QDoubleSpinBox(this);
    m_pSlantsSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pSlantsSpin->setFixedHeight(OneParamWidgetHeiht);
    m_pSlantsSpin->setRange(-360, 360);
    m_pSlantsSpin->setValue(0);

    // 创建显示文字“Tilt:”的标签
    m_pTiltsLabel = new QLabel(this);
    m_pTiltsLabel->setText(tr("Tilt:"));
    m_pTiltsLabel->setFixedHeight(OneParamWidgetHeiht);

    // 创建输入 Tilt 数值的小数数字框
    m_pTiltsSpin = new QDoubleSpinBox(this);
    m_pTiltsSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTiltsSpin->setFixedHeight(OneParamWidgetHeiht);
    m_pTiltsSpin->setRange(-360, 360);
    m_pTiltsSpin->setValue(0);

    //创建加载图片的按钮
    m_pLoadImageBtn = new QPushButton(this);
    m_pLoadImageBtn->setText(tr("load"));//设置按钮上的文字
    m_pLoadImageBtn->setFixedSize(80, OneParamWidgetHeiht);//固定按钮宽度80,高度和前面组件一样
    m_pLoadImageBtn->setIconSize(QSize(24, 24));// 设置按钮内部图标的尺寸为 24x24 像素
    m_pLoadImageBtn->setIcon(QIcon(":/Resouce/icon/loadimg.png"));

    connect(m_pLoadImageBtn,&QPushButton::clicked, this,&ZZOneParamWidget::OnSigLoadImageBtnClicked);

    //创建布局
    QHBoxLayout* pTitleLayout = new QHBoxLayout(this);
    pTitleLayout->setContentsMargins(0,0,0,0);
    pTitleLayout->setSpacing(0);
    pTitleLayout->addWidget(m_pTitleLabel);
    pTitleLayout->addStretch();
    QWidget* pTilteWidget = new QWidget(this);
    pTilteWidget->setLayout(pTitleLayout);

    QHBoxLayout* pParamLayout = new QHBoxLayout(this);
    pParamLayout->setContentsMargins(0,0,0,0);
    pParamLayout->setSpacing(0);
    pParamLayout->addWidget(m_pSlantsLabel);
    pParamLayout->addWidget(m_pSlantsSpin);
    pParamLayout->addSpacing(8);
    pParamLayout->addWidget(m_pTiltsLabel);
    pParamLayout->addWidget(m_pTiltsSpin);
    pParamLayout->addSpacing(8);
    pParamLayout->addWidget(m_pLoadImageBtn);
    QWidget* pParamWidget = new QWidget(this);
    pParamWidget->setLayout(pParamLayout);

    QVBoxLayout*pMainLayout = new QVBoxLayout(this);
    pMainLayout->setContentsMargins(0,0,0,0);
    pMainLayout->setSpacing(0);
    pMainLayout->addWidget(pTilteWidget);
    pMainLayout->addWidget(pParamWidget);

    this->setLayout(pMainLayout);
    return true;
}

QImage ZZOneParamWidget::GetQimage()
{
    return m_qImage;
}

//设置倾斜角度
void ZZOneParamWidget::SetSlantAngle(float fSlant)
{
    if(m_pSlantsSpin)//如果这个输入框指针存在
    {
        m_pSlantsSpin->setValue(fSlant);//把输入框里的数字，改成传进来的 fSlant
    }
}

//获取倾斜角度
float ZZOneParamWidget::GetSlantAngle()
{
    return m_pSlantsSpin->value();
}

void ZZOneParamWidget::SetTiltAngle(float fTilt)
{
    if(m_pTiltsSpin)
    {
        m_pTiltsSpin->setValue(fTilt);
    }
}

float ZZOneParamWidget::GetTiltAngle()
{
    return m_pTiltsSpin->value();
}

void ZZOneParamWidget::Reset()
{
    //m_pSlantsSpin->setValue(0.0);
    //m_pTiltsSpin->setValue(0.0);

    m_qImage = QImage();
}

void ZZOneParamWidget::OnSigLoadImageBtnClicked(bool clicked)
{
    Q_UNUSED(clicked);
    QString strImgPath = QFileDialog::getOpenFileName(
        this, tr("Select Image"),
        "D:\\Visual Studio3\\ZhuZhaoGUI\\PhotometricStereo\\images",
        tr("Images(*.jpg *.png *.bmp)"));
    if(strImgPath.isNull())
    {
        return;
    }
    m_qImage = QImage(strImgPath);
    if(m_qImage.isNull())
    {
        QMessageBox::information(this, tr("Error"), tr("Load Image Failed!"));
        return;
    }
    ListenerManger::Instance()->notify(ZHUZHAO_UPDATE_SRCIMAGE);
}