#include "VThumbnailList.h"
#include <QLayout>
#include <QFileDialog>
#include <QScrollBar>
#include "LitImgItemWidget.h" // 注意：如果要编译成功，这个头文件的注释需要打开

// 构造函数：在外部通过 new 创建本面板时，系统第一时间自动执行
VThumbnailList::VThumbnailList(QWidget *parent)
    : QWidget{parent}
    , m_pListWidget(nullptr) // 将指针初始地址设为空，防止野指针报错
    , m_bIsRunOnce(false)    // 初始化状态开关为假
{
    // 立刻调用初始化函数搭建界面。如果搭建失败，强制抛出内存分配错误并终止
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

// 析构函数：在这个面板被关闭或彻底销毁时自动执行
VThumbnailList::~VThumbnailList()
{
    // 因为子控件都绑定了 this 指针，Qt 底层会自动回收它们占用的内存，这里无需手写 delete
}

bool VThumbnailList::InitWidget()
{
    // 1. 在物理内存中真实创建出原生的列表控件容器
    m_pListWidget = new QListWidget(this);

    // 2. 使用 QSS 纯文本语法，强制修改滚动条的显卡渲染规则（设为透明轨道和圆角滑块）
    m_pListWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:5px; margin:0px 0px 0px 0px;}"
                                                        "QScrollBar::handle{background:rgba(223, 223, 225, 200); border:0px; border-radius:5px; margin:0px 0px 0px 0px;}"
                                                        "QScrollBar::handle:hover{background:lightgray;}"
                                                        "QScrollBar::handle:pressed{background:rgba(200, 200, 200, 255);}"
                                                        "QScrollBar::sub-page{background:transparent;}"
                                                        "QScrollBar::add-page{background:transparent;}"
                                                        "QScrollBar::up-arrow{background:transparent;}"
                                                        "QScrollBar::down-arrow{background:transparent;}"
                                                        "QScrollBar::sub-line{background:transparent; height:0px;}"
                                                        "QScrollBar::add-line{background:transparent; height:0px;}");

    // 3. 设定控件随外部面板拉伸的策略
    m_pListWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    // 4. 强制开启垂直滚动条（需要时显示），彻底关闭水平滚动条
    m_pListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 5. 制定列表内部元素的排版与交互死命令
    m_pListWidget->setFlow(QListView::TopToBottom); // 强制从上到下排列
    m_pListWidget->setViewMode(QListView::ViewMode::ListMode); // 列表视图模式
    m_pListWidget->setSelectionMode(QListView::SingleSelection); // 限制只能单选
    m_pListWidget->setFocusPolicy(Qt::NoFocus); // 消除系统默认的虚线焦点框
    m_pListWidget->setSelectionRectVisible(false); // 隐藏默认的高亮背景块
    m_pListWidget->setVerticalScrollMode(QListView::ScrollPerPixel); // 开启像素级平滑滚动
    m_pListWidget->setDragEnabled(false); // 绝对禁止鼠标拖拽图片改变顺序
    m_pListWidget->setSpacing(4); // 强制每张图片之间留出 4 像素缝隙

    // 6. 实例化垂直布局管理器，并将四周边缘缝隙全部设为 0
    QVBoxLayout* pListLayout = new QVBoxLayout();
    pListLayout->setSpacing(0);
    pListLayout->setContentsMargins(0,0,0,0);

    // 7. 把列表塞进布局，再把布局套在当前面板上，实现列表 100% 撑满面板
    pListLayout->addWidget(m_pListWidget);
    this->setLayout(pListLayout);

    // 8.只要原生列表的高亮行发生改变，立刻强制启动 OnCurrentRowChanged 函数
    connect(m_pListWidget,&QListWidget::currentRowChanged,
            this,&VThumbnailList::OnCurrentRowChanged);

    return true;
}

// 跨模块通信发射：受系统监控，行号一变自动执行
void VThumbnailList::OnCurrentRowChanged(int nCurRow)
{
    // 安全拦截：防止越界
    if(nCurRow < 0 || nCurRow >= m_listImage.size())
        return;

    // 从私有仓库里提取最新点中的那张图的真实像素
    QImage curImage = m_listImage[nCurRow];

    // 利用 emit 发起全软件广播，把真实图片数据当作包裹向外发射
    emit SigSelectImageChanged(curImage);
}

//1 图片添加功能
void VThumbnailList::addImage(const QString& strPath)
{
    // 利用外部传来的硬盘路径文本，去硬盘提取像素并在内存生成图片数据包
    QImage qImage(strPath);
    // 将生成的数据包转交给负责干活的同名函数
    addImage(qImage);
}

//2 图片添加功能
void VThumbnailList::addImage(QImage& qImage)
{
    // 1. 在内存中造一个我们自己写的、带红黑边框绘图功能的自定义面板，并把像素数据塞进去
    LitImgItemWidget *pImgItemWidget = new LitImgItemWidget();
    pImgItemWidget->setImage(qImage);

    // 2. 造一个 Qt 原生列表专用的透明占位方格，强制设定成正方形尺寸
    QListWidgetItem *pItem = new QListWidgetItem();
    pItem->setSizeHint(QSize(this->width() - 10,this->width() - 10));

    // 3. 把透明方格按顺序加入列表中
    m_pListWidget->addItem(pItem);

    // 4. 用刚才造好的自定义面板，强行覆盖嵌入到这个透明方格上，实现画面显示
    m_pListWidget->setItemWidget(pItem,pImgItemWidget);

    // 5. 物理备份：将这张大图的真实像素数据，死死存入私有集合中供日后提取
    m_listImage.push_back(qImage);
}

//3 图片添加功能
void VThumbnailList::addImages(QList<QImage>& qImages)
{
    // 利用 for 循环，把传进来的一大堆图片逐个拆解
    for(QImage& img : qImages)
        addImage(img);
}

// 呼出系统窗口读取文件
// void VThumbnailList::openFolder()
// {
//     // 1. 调用操作系统底层的弹窗接口，生成文件选择器
//     QFileDialog *fileDialog = new QFileDialog(this);
//     fileDialog->setWindowTitle(tr("OpenImageFolder")); // 窗口标题
//     fileDialog->setDirectory("."); // 默认打开程序的当前运行目录
//     fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)")); // 限制只显示图片后缀的文件
//     fileDialog->setFileMode(QFileDialog::ExistingFiles); // 允许用户按 Ctrl 框选多个文件
//     fileDialog->setViewMode(QFileDialog::Detail); // 显示文件的详细信息视图

//     // 2. 如果用户点击了“确定”（exec() 返回 true）
//     if(fileDialog->exec())
//     {
//         // 强制清空旧数据，防止新老图片混杂
//         clearAllImages();

//         // 把用户刚才选中的所有文件的绝对路径提取到内存中
//         m_strImagePaths = fileDialog->selectedFiles();

//         // 遍历这些路径，逐一发送给“1”去读取并显示
//         for(auto strImagePath : m_strImagePaths)
//         {
//             addImage(strImagePath);
//         }
//     }
// }

void VThumbnailList::clearAllImages()
{
    m_pListWidget->clear();    // 抹除界面屏幕上的所有可视方格
    m_strImagePaths.clear();   // 清空内存里的路径文本备份
    m_listImage.clear();       // 清空内存里的真实像素备份，释放内存空间
}

// 纯物理动作：高亮光标向下跳
void VThumbnailList::nextImage()
{
    // 获取当前红框所在的行号
    int nCurrRow = m_pListWidget->currentRow();

    // 边界拦截：如果当前行号等于图片总数减1（说明已经到了最底下最后一张图）
    if(nCurrRow == m_listImage.size() - 1)
    {
        nCurrRow = -1; // 强行把数字改成 -1
    }

    // 强制命令界面将高亮光标设定在当前数字 + 1 的位置
    // 如果刚才到底了，这里就是 -1 + 1 = 0，瞬间跳回顶部第一张
    m_pListWidget->setCurrentRow(nCurrRow + 1);
}

// 纯物理动作：高亮光标向上跳
void VThumbnailList::lastImage()
{
    // 获取当前红框所在的行号
    int nCurrRow = m_pListWidget->currentRow();

    // 边界拦截：如果当前已经是第 0 行（最顶部第一张图）
    if(nCurrRow == 0)
    {
        nCurrRow = m_pListWidget->count(); // 强行把数字改成图片的总数量
    }

    // 强制命令界面将光标设定在当前数字 - 1 的位置
    // 如果刚才到顶了，总数量 - 1 刚好就是最后一张图的行号，瞬间跳回最底部
    m_pListWidget->setCurrentRow(nCurrRow - 1);
}

// 复合动作：往下翻一页，并把翻页后的图片提取出来
QImage VThumbnailList::getNextImage()
{
    this->nextImage(); // 先执行向下跳的纯动作
    return getCurrentImage(); // 立刻去仓库里把跳完之后的图片像素取出来
}

// 复合动作：往上翻一页，并把翻页后的图片提取出来
QImage VThumbnailList::getLastImage()
{
    this->lastImage(); // 先执行向上跳的纯动作
    return getCurrentImage(); // 立刻取数据
}

// 核心提货员：提取当前红框对应的真实像素
QImage VThumbnailList::getCurrentImage()
{
    // 问界面：你现在高亮的是第几行？
    int nCurrRow = m_pListWidget->currentRow();

    // 安全拦截：防止没选中任何图或数字出错导致程序提取越界崩溃
    if(nCurrRow < 0 || nCurrRow >= m_listImage.length())
    {
        return QImage(); // 返回一张空的假图片
    }

    // 拿着正确的行号，直接去私有内存仓库里把那包真实的大图像素甩出去
    return m_listImage[nCurrRow];
}
