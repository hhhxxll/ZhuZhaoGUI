#include "ZZProcessThread.h"
#include "PhotometricStereo.h"
#include "ImageConvert.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

ZZProcessThread::ZZProcessThread() {}

void ZZProcessThread::SetPhotometricStereoParams(QList<QImage>& srcImages,
                                                 QList<float>& Slants,
                                                 QList<float>& Tilts)
{
    QMutexLocker locker(&m_mutex);
    m_srcImages = srcImages;
    m_Slants = Slants;
    m_Tilts = Tilts;
}

void ZZProcessThread::GetResultImage(QList<QImage>& dstImages)
{
    QMutexLocker locker(&m_mutex);
    dstImages = m_dstImages;
}

void ZZProcessThread::run()
{
    QList<QImage> srcImages;
    QList<float> srcSlants;
    QList<float> srcTilts;

    {
        QMutexLocker locker(&m_mutex);
        srcImages = m_srcImages;
        srcSlants = m_Slants;
        srcTilts = m_Tilts;
    }

    std::vector<cv::Mat> cvSrcImages;
    std::vector<float> cvSrcSlants;
    std::vector<float> cvSrcTilts;
    cv::Mat dstHeightFieldImg;
    cv::Mat dstGradientImg;
    cv::Mat dstAlbedoImg;

    for(int i = 0; i < srcImages.size(); i++)
    {
        QImage qsrcImage = srcImages.at(i).convertToFormat(QImage::Format_Grayscale8);
        cv::Mat srcImage = QImage2cvMat(qsrcImage);
        cvSrcImages.push_back(srcImage);
        cvSrcSlants.push_back(srcSlants.at(i));
        cvSrcTilts.push_back(srcTilts.at(i));
    }

    ZhuZhao::PhotometricStereo(
        cvSrcImages,
        dstHeightFieldImg,
        dstGradientImg,
        dstAlbedoImg,
        cvSrcImages.size(),
        cvSrcSlants,
        cvSrcTilts
        );

    {
        QMutexLocker locker(&m_mutex);
        m_dstImages.clear();
        m_dstImages.push_back(cvMat2QImage(dstHeightFieldImg));
        m_dstImages.push_back(cvMat2QImage(dstGradientImg));
        m_dstImages.push_back(cvMat2QImage(dstAlbedoImg));
    }
}