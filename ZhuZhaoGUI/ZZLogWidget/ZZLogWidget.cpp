#include "ZZLogWidget.h"
#include "ZZLogMessage.h"
#include <QLayout>
#include <QScrollBar>
#include <QDesktopServices>

ZZLogWidget::ZZLogWidget(QWidget *parent)
    : QWidget{parent}
    , m_pClearBtn(Q_NULLPTR)
    , m_pHelperBtn(Q_NULLPTR)
    , m_pLogTextBrowser(Q_NULLPTR)
{
    this->setMinimumSize(300,200);
    if(InitWidget() == false)
    {
        throw std::bad_alloc();
    }
}

bool ZZLogWidget::InitWidget()
{
    m_pClearBtn = new QPushButton(this);
    m_pClearBtn->setText(tr("ClearLog"));
    m_pClearBtn->setFixedSize(120,28);
    m_pClearBtn->setIconSize(QSize(24,24));
    m_pClearBtn->setIcon(QIcon(":/Resouce/icon/clear.png"));

    m_pHelperBtn = new QPushButton(this);
    m_pHelperBtn->setText(tr("Helper"));
    m_pHelperBtn->setFixedHeight(28);
    m_pHelperBtn->setIconSize(QSize(24,24));
    m_pHelperBtn->setIcon(QIcon(":/Resouce/icon/helper.png"));
    m_pHelperBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pHelperBtn->setStyleSheet("QPushButton{text-align:left;padding-left:12px;}");

    m_pLogTextBrowser = new QTextBrowser(this);

    //创建信号槽
    connect(m_pClearBtn, &QPushButton::clicked, this,&ZZLogWidget::OnClearBtnClicked);
    connect(m_pHelperBtn, &QPushButton::clicked, this,&ZZLogWidget::OnHelperBtnClicked);
    //绑定信号，将调试信息输出值ui
    connect(ZZLogMessage::Instance(), &ZZLogMessage::sigDebugHtmlData, m_pLogTextBrowser, &QTextBrowser::append);

    //初始化布局
    QHBoxLayout* pBtnLayout = new QHBoxLayout(this);    //水平布局
    pBtnLayout->setContentsMargins(0,0,0,0);
    pBtnLayout->setSpacing(0);
    pBtnLayout->addWidget(m_pHelperBtn);
    pBtnLayout->addWidget(m_pClearBtn);
    QWidget* pBtnWidget = new QWidget(this);
    pBtnWidget->setLayout(pBtnLayout);
    pBtnWidget->setFixedHeight(32);

    QVBoxLayout* m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setContentsMargins(0,0,0,0);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->addWidget(m_pLogTextBrowser);
    m_pMainLayout->addWidget(pBtnWidget);
    this->setLayout(m_pMainLayout);

    return true;
}

void ZZLogWidget::OnClearBtnClicked()
{
    m_pLogTextBrowser->clear();
    QDEBUG("清空日志显示");
}

void ZZLogWidget::OnHelperBtnClicked()
{
    QDesktopServices::openUrl(QUrl(""));
}