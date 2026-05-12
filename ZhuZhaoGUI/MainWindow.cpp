 #include "MainWindow.h"
#include <QLayout>  // 引入 Qt 布局管理类头文件
#include <QImage>   // 引入 Qt 图片处理类头文件
#include <QSplitter>// 引入 Qt 分割窗口控件头文件
#include "ZZConfigWidget/ZZConfigWidget.h"
#include "ZZConfigWidget/ZZProcessThread.h"
#include "ZZLogWidget/ZZLogWidget.h"
#include "ZZViewWidget/CustomImageView.h"
#include "ZZThumWidget/VThumbnailList.h"
#include "ZZThumWidget/HThumbnailList.h"

//初始化主窗口
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ZZListener()
    , m_pConfigWidget(Q_NULLPTR)
    , m_pLogWidget(Q_NULLPTR)
    , m_pImageView(Q_NULLPTR)
    , m_pHThumList(Q_NULLPTR)
    , m_pVThumList(Q_NULLPTR)
    , m_pRunProcess(Q_NULLPTR)
{
    this->setMinimumSize(1000,600);// 设定窗口的最小尺寸限制

    this->setWindowTitle(tr("ZhuzhaoGUI"));// 设定软件左上角显示的文字标题

    if(false == InitWidget())// 判断初始化控件函数是否执行失败
    {
        throw std::bad_alloc();//  如果失败就抛出内存分配异常
    }

    // 调用监听管理器的实例，把当前主窗口（this）注册进去
    ListenerManger::Instance()->registerMessage(
        MESSAGE::ZHUZHAO_UPDATE_SRCIMAGE | // 监听“更新源图像”消息
            MESSAGE::ZHUZHAO_UPDATE_RESULTI |  // 监听“更新结果图像”消息
            MESSAGE::ZHUZHAO_RUNONCE           // 监听“单次运行”消息
        , this);

    //首次初始化先更新一下图像列表
    ListenerManger::Instance()->notify(ZHUZHAO_UPDATE_SRCIMAGE);
}

MainWindow::~MainWindow()
{
    ListenerManger::Instance()->unregisterAll(this);
}

//初始化组件
bool MainWindow::InitWidget()
{
    QFont font("Microsoft YaHei", 20);// 设置字体
    font.setBold(true);//加粗

    m_pConfigWidget = new ZZConfigWidget(this);// 创建配置界面的控件
    m_pLogWidget = new ZZLogWidget(this);// 创建日志界面的控件
    m_pImageView = new CustomImageView(this);// 创建图片界面的控件

    m_pHThumList = new HThumbnailList(this);// 创建横向的缩略图列表
    m_pHThumList->setFixedHeight(120);//固定高度
    m_pVThumList = new VThumbnailList(this);// 创建纵向的缩略图列表
    m_pVThumList->setFixedWidth(120);//固定宽度

    // 将横向和纵向列表的“图片被选中”信号，连接到图片查看区的“显示图片”功能上
    connect(m_pHThumList,&HThumbnailList::SigSelectImageChanged,m_pImageView,&CustomImageView::OnSendImage);
    connect(m_pVThumList,&VThumbnailList::SigSelectImageChanged,m_pImageView,&CustomImageView::OnSendImage);

    // 实际创建出那个专门在后台运行处理任务的工具
    m_pRunProcess = new ZZProcessThread();
    // 将后台任务的“运行结束”信号，连接到主窗口的“处理完毕”接收口上
    connect(m_pRunProcess,&ZZProcessThread::finished, this,&MainWindow::OnProcessThreadFinished);

    //创建布局
    //  1.创建一个上下布局的可拖动分割栏，把配置界面和日志界面放在同一个分割器里
    QSplitter* pLeftSplitter = new QSplitter(Qt::Vertical);
    pLeftSplitter->addWidget(m_pConfigWidget);
    pLeftSplitter->addWidget(m_pLogWidget);

    //创建一个垂直布局管理器，将图片查看区和横向的缩略图列表从上到下垂直排列，并在底部加弹簧
    QVBoxLayout* pViewLayout = new QVBoxLayout();
    pViewLayout->setContentsMargins(0,0,0,0);
    pViewLayout->addWidget(m_pImageView);
    pViewLayout->addWidget(m_pHThumList);
    pViewLayout->addStretch();

    //创建水平排版布局，把缩略列表和另一个子布局放在一起,装到一个界面容器上
    QHBoxLayout* pRightLayout = new QHBoxLayout();
    pRightLayout->setContentsMargins(0,0,0,0);
    pRightLayout->addWidget(m_pVThumList);
    pRightLayout->addLayout(pViewLayout);
    QWidget* pRightWidget = new QWidget(this);
    pRightWidget->setLayout(pRightLayout);

    //创建了一个水平的分割器，左边塞入“配置+日志”，右边塞入“全部图片与列表”。
    QSplitter* pMianSplitter = new QSplitter(Qt::Horizontal);
    pMianSplitter->addWidget(pLeftSplitter);
    pMianSplitter->addWidget(pRightWidget);

    //创建一个水平布局,把pMianSplitter放进去,然后在放到主窗口pCenterWidget里
    QHBoxLayout* pMainLayout = new QHBoxLayout();
    pMainLayout->setContentsMargins(0,0,0,0);
    pMainLayout->addWidget(pMianSplitter);
    QWidget* pCenterWidget = new QWidget(this);
    pCenterWidget->setLayout(pMainLayout);

    // 把这块底板设定为当前主窗口的核心显示区域
    this->setCentralWidget(pCenterWidget);
    return true;
}

//进程线程处理完成
void MainWindow::OnProcessThreadFinished()
{
    QList<QImage> dstImages;
    m_pRunProcess->GetResultImage(dstImages);
    m_pHThumList->clearAllImages();
    m_pHThumList->addImage(dstImages);
}

void MainWindow::RespondMessage(int message)
{
    //刷新输入图像列表
    if((message & MESSAGE::ZHUZHAO_UPDATE_SRCIMAGE) == MESSAGE::ZHUZHAO_UPDATE_SRCIMAGE)
    {
        QList<QImage> srcImages;
        QList<float> Slants;
        QList<float> Tilts;
        m_pConfigWidget->GetPhotometricStereoParams(srcImages,Slants,Tilts);
        m_pVThumList->clearAllImages();
        m_pVThumList->addImages(srcImages);
    }
    //更新结果图
    if((message & MESSAGE::ZHUZHAO_UPDATE_RESULTI) == MESSAGE::ZHUZHAO_UPDATE_RESULTI)
    {

    }
    //运行算法
    if((message & MESSAGE::ZHUZHAO_RUNONCE) == MESSAGE::ZHUZHAO_RUNONCE)
    {
        if(m_pRunProcess->isRunning())
            return;

        QList<QImage> srcImages;
        QList<float> Slants;
        QList<float> Tilts;
        m_pConfigWidget->GetPhotometricStereoParams(srcImages,Slants,Tilts);
        m_pRunProcess->SetPhotometricStereoParams(srcImages,Slants,Tilts);
        m_pRunProcess->start();
    }
}