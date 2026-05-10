#ifndef ZZPROCESSTHREAD_H
#define ZZPROCESSTHREAD_H

#include <QThread>
#include <QImage>
#include <QMutex>

class ZZProcessThread : public QThread
{
public:
    ZZProcessThread();
    void SetPhotometricStereoParams(QList<QImage>& srcImage,
                                    QList<float>& Slants,
                                    QList<float>& Tilts);

    void GetResultImage(QList<QImage>& dstImages);

protected:
    void run() override;
private:
    QMutex m_mutex;
    QList<QImage> m_srcImages;
    QList<QImage> m_dstImages;
    QList<float> m_Slants;
    QList<float> m_Tilts;
};

#endif // ZZPROCESSTHREAD_H
