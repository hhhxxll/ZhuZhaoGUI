#ifndef IMAGECONVERT_H
#define IMAGECONVERT_H
#include <QtCore/QDebug>
#include <QtGui/QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

/*
 * cv::Mat 与 QImage 之间的转换函数
 * OpenCV 使用 BGR 颜色顺序，Qt 使用 RGB 颜色顺序
 * 两个函数处理了这个差异，实现双向转换
 */

/*
 * @brief 将 OpenCV 的 cv::Mat 转换为 Qt 的 QImage
 * @param mat      待转换的图像，支持 CV_8UC1(灰度)、CV_8UC3(彩色)、CV_8UC4(带透明度彩色)
 * @param clone    true=深拷贝(独立内存)，false=共享内存(修改会互相影响)
 * @param rb_swap  仅对 CV_8UC3 有效，true=将 BGR 转为 RGB，false=保持原样
 * @return 转换后的 QImage，失败返回空 QImage
 */
QImage cvMat2QImage(const cv::Mat& mat, bool clone = true, bool rb_swap = true)
{
    // 获取 Mat 的像素数据指针，转为 uchar* 类型
    const uchar *pSrc = (const uchar*)mat.data;

    // 灰度图：单通道，每个像素 1 字节
    if(mat.type() == CV_8UC1)
    {
        // 用 Mat 的数据直接构造 QImage，不复制内存
        // 参数：数据指针, 宽, 高, 每行字节数(含对齐), 像素格式
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        if(clone) return image.copy();  // 深拷贝，返回独立副本
        return image;                   // 共享内存，直接返回
    }
    // 彩色图：3通道，每个像素 3 字节 (BGR)
    else if(mat.type() == CV_8UC3)
    {
        // 构造 QImage，格式为 RGB888
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        if(clone)
        {
            // 深拷贝模式
            if(rb_swap) return image.rgbSwapped();  // 拷贝的同时交换 R 和 B 通道
            return image.copy();                     // 只拷贝，不交换
        }
        else
        {
            // 共享内存模式
            if(rb_swap)
            {
                // 直接修改原 Mat 的内存，将 BGR 转为 RGB
                // 因为共享内存，QImage 也会同步变化
                cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            }
            return image;
        }
    }
    // 带透明度的彩色图：4通道，每个像素 4 字节 (BGRA)
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // BGRA 和 ARGB32 内存布局兼容，无需交换通道
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        if(clone) return image.copy();
        return image;
    }
    else
    {
        // 不支持的格式，返回空图像
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

/*
 * @brief 将 Qt 的 QImage 转换为 OpenCV 的 cv::Mat
 * @param image    待转换的图像，支持灰度、RGB888、ARGB32 等格式
 * @param clone    true=深拷贝(独立内存)，false=共享内存(修改会互相影响)
 * @param rb_swap  仅对 RGB888 格式有效，true=将 RGB 转为 BGR，false=保持原样
 * @return 转换后的 cv::Mat
 */
cv::Mat QImage2cvMat(QImage &image, bool clone = true, bool rb_swap = true)
{
    cv::Mat mat;
    qDebug() << image.format();

    switch(image.format())
    {
    // 32位带透明度格式：4通道
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        // 用 QImage 的数据构造 Mat，不复制内存
        // 参数：高, 宽, 类型, 数据指针, 每行字节数
        mat = cv::Mat(image.height(), image.width(), CV_8UC4,
                      (void *)image.constBits(), image.bytesPerLine());
        if(clone) mat = mat.clone();  // 深拷贝
        break;

    // RGB888 格式：3通道彩色
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3,
                      (void *)image.constBits(), image.bytesPerLine());
        if(clone) mat = mat.clone();  // 先深拷贝
        if(rb_swap)
        {
            // 将 RGB 转为 BGR（OpenCV 的默认顺序）
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        }
        break;

    // 灰度格式：单通道
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1,
                      (void *)image.bits(), image.bytesPerLine());
        if(clone) mat = mat.clone();
        break;
    }
    return mat;
}

#endif // IMAGECONVERT_H
