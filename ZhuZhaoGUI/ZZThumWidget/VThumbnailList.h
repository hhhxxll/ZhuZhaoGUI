#ifndef VTHUMBNAILLIST_H
#define VTHUMBNAILLIST_H

#include <QWidget>
#include <QListWidget>  // 导入 Qt 自带列表控件的底层代码
#include <QList>
#include <QImage>

class VThumbnailList : public QWidget
{
    Q_OBJECT
public:
    VThumbnailList(QWidget *parent = nullptr);
    ~VThumbnailList();

    bool InitWidget();

    //传入路径，让列表加载图片
    void addImage(const QString& strPath);

    //传入一张图片
    void addImage(QImage& qImage);

    //添加多张图像
    void addImages(QList<QImage>& qImages);

    //打开图像文件夹
    void openFolder();

    //清除所有图像
    void clearAllImages();

    //获取下张图像
    void nextImage();

    //获取上张图像
    void lastImage();

    //获取下张图像
    QImage getNextImage();

    //获取上张图像
    QImage getLastImage();

    //获取当前图像
    QImage getCurrentImage();

signals:
    void SigSelectImageChanged(QImage& qImage);
protected slots:
    void OnCurrentRowChanged(int nCurRow);

private:
    QListWidget* m_pListWidget;
    QStringList m_strImagePaths;
    QList<QImage> m_listImage;
    bool m_bIsRunOnce;
};

#endif // VTHUMBNAILLIST_H
