#include "ZZConfigWidget.h"
#include "ZZOneParamWidget.h"
#include "ZZListener.h"
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>

ZZConfigWidget::ZZConfigWidget(QWidget *parent)
    : QWidget{parent}
    , m_pTitleLabel(Q_NULLPTR)
    , m_pResetBtn(Q_NULLPTR)
    , m_pRunOnceBtn(Q_NULLPTR)
{
    this->setMinimumSize(400, 300);
    this->setAutoFillBackground(true);
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

bool ZZConfigWidget::InitWidget()
{
    auto CreateOneParam = [&](QString strParamName)->ZZOneParamWidget*
    {
        auto pParamWidget = new ZZOneParamWidget(strParamName, this);
        pParamWidget->setFixedHeight(60);
        m_ParamWidgetList.push_back(pParamWidget);

        return pParamWidget;
    };

    //创建标题
    QFont font("Microsoft YaHei", 12);
    font.setBold(true);//加粗字体

    m_pTitleLabel = new QLabel(this);//创建一个文字标签控件
    m_pTitleLabel->setText(tr("Config Widget"));
    m_pTitleLabel->setFont(font);//应用上面配好的到标签上
    m_pTitleLabel->setFixedHeight(36);//设置标签高度36

    m_pResetBtn = new QPushButton(this);
    m_pResetBtn->setText(tr("Reset"));
    m_pResetBtn->setFixedSize(90, 28);
    m_pResetBtn->setIconSize((QSize(24, 24)));
    m_pResetBtn->setIcon(QIcon(":/Resouce/icon/reset.png"));
    connect(m_pResetBtn,&QPushButton::clicked,this,&ZZConfigWidget::OnResetBtnClicked);

    m_pRunOnceBtn = new QPushButton(this);
    m_pRunOnceBtn->setText(tr("RunOnce"));
    m_pRunOnceBtn->setFixedSize(90, 28);
    m_pRunOnceBtn->setIconSize(QSize(24,24));
    m_pRunOnceBtn->setIcon(QIcon(":/Resouce/icon/runonce.png"));
    connect(m_pRunOnceBtn,&QPushButton::clicked,this,&ZZConfigWidget::OnRunOnceBtnClicked);

    //创建按钮布局
    QHBoxLayout* pBtnLayout = new QHBoxLayout(this);//H代表横向布局
    pBtnLayout->setContentsMargins(0,0,0,0);
    pBtnLayout->addStretch();//添加弹簧在最左边
    pBtnLayout->addWidget(m_pResetBtn);
    pBtnLayout->addSpacing(8);
    pBtnLayout->addWidget(m_pRunOnceBtn);
    QWidget* pBtnWidget = new QWidget(this);
    pBtnWidget->setLayout(pBtnLayout);
    pBtnWidget->setFixedHeight(36);
    pBtnWidget->setStyleSheet("background-color:darkgray");

    //创建参数布局
    QHBoxLayout* pTitleLayout = new QHBoxLayout(this);
    pTitleLayout->setContentsMargins(0,0,0,0);
    pTitleLayout->addWidget(m_pTitleLabel);
    pTitleLayout->addStretch();
    QWidget* pTilteWidget = new QWidget(this);
    pTilteWidget->setLayout(pTitleLayout);

    //创建单个参数窗口
    auto pFirstParamWidget = CreateOneParam(tr("FirstParam"));
    auto pSecondParamWidget = CreateOneParam(tr("SecondParam"));
    auto pThirdParamWidget = CreateOneParam(tr("ThirdParam"));
    auto pFourthParamWidget = CreateOneParam(tr("FourthParam"));

    //将角度参数的初始值写死在代码内
    pFirstParamWidget->SetTiltAngle(6.1);
    pFirstParamWidget->SetSlantAngle(41.4);
    pSecondParamWidget->SetTiltAngle(95.0);
    pSecondParamWidget->SetSlantAngle(42.6);
    pThirdParamWidget->SetTiltAngle(-176.1);
    pThirdParamWidget->SetSlantAngle(41.7);
    pFourthParamWidget->SetTiltAngle(-86.8);
    pFourthParamWidget->SetSlantAngle(40.9);

    QVBoxLayout* pTopLayout = new QVBoxLayout(this);
    pTopLayout->setContentsMargins(10,10,10,0);
    pTopLayout->setSpacing(0);
    pTopLayout->addWidget(pTilteWidget);
    pTopLayout->addWidget(pFirstParamWidget);
    pTopLayout->addWidget(pSecondParamWidget);
    pTopLayout->addWidget(pThirdParamWidget);
    pTopLayout->addWidget(pFourthParamWidget);
    QWidget* pTopWidget = new QWidget(this);
    pTopWidget->setLayout(pTopLayout);
    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    pMainLayout->setContentsMargins(0,0,0,0);
    pMainLayout->setSpacing(0);
    pMainLayout->addWidget(pTopWidget);
    pMainLayout->addStretch();
    pMainLayout->addWidget(pBtnWidget);

    this->setLayout(pMainLayout);
    return true;
}

void ZZConfigWidget::GetPhotometricStereoParams(QList<QImage>& srcImages,
                                                 QList<float>& Slants,
                                                 QList<float>& Tilts)
{
    srcImages.clear();
    Slants.clear();
    Tilts.clear();
    for(auto pParamWidget : m_ParamWidgetList)
    {
        QImage srcImage = pParamWidget->GetQimage();
        float slantAngle = pParamWidget->GetSlantAngle();
        float tiltAngle = pParamWidget->GetTiltAngle();

        srcImages.push_back(srcImage);
        Slants.push_back(slantAngle);
        Tilts.push_back(tiltAngle);
    }
}

void ZZConfigWidget::OnResetBtnClicked(bool clicked)
{
    Q_UNUSED(clicked);
    for(auto pParamWidget : m_ParamWidgetList)
    {
        pParamWidget->Reset();
    }
    ListenerManger::Instance()->notify(ZHUZHAO_UPDATE_SRCIMAGE);
}

void ZZConfigWidget::OnRunOnceBtnClicked(bool clicked)
{
    Q_UNUSED(clicked);
    ListenerManger::Instance()->notify(ZHUZHAO_RUNONCE);
}