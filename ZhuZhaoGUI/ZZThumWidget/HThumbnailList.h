#ifndef HTHUMBNAILLIST_H
#define HTHUMBNAILLIST_H

#include <QWidget>
#include <QListWidget>
#include <QList>
#include <QImage>

class HThumbnailList : public QWidget
{
    Q_OBJECT
public:
    HThumbnailList(QWidget *parent = nullptr);
    ~HThumbnailList();
    bool InitWidget();

    void addImage(const QString& strPath);
    void addImage(QImage& qImage);
    void addImage(QList<QImage>& qImage);
//    void openFolder();
    void clearAllImages();
    void lastImage();
    void nextImage();
    QImage getNextImage();
    QImage getLastImage();
    QImage getCurrentImage();

signals:
    void SigSelectImageChanged(QImage& qImage);
protected slots:
    void OnCurrentRowChanged(int nCurRow);

private:
    QListWidget* m_pListWidget;
    QStringList m_strImagePaths;//存储缩略图读取的图像的绝对路径
    QList<QImage> m_listImage;//存储图像列表
    bool m_bIsRunOnce;//标识是手动选中图像，还是next图像引起的图像列表变化

};

#endif // HTHUMBNAILLIST_H
