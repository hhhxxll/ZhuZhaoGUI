#ifndef ZZLISTENER_H
#define ZZLISTENER_H

#include <QMap>
#include <QVector>
#include <QMutex>

enum MESSAGE {
    ZHUZHAO_UPDATE_SRCIMAGE	= 0x00000001,//刷新输入图像列表
    ZHUZHAO_UPDATE_RESULTI  = 0x00000002,//刷新算法运行结果
    ZHUZHAO_RESET           = 0x00000003,//重置界面参数
    ZHUZHAO_RUNONCE         = 0x00000004,//单次运行一次
};

class ZZListener
{
public:
    ZZListener();
    virtual ~ZZListener() {};
    virtual void RespondMessage(int message) = 0;
};

class ListenerManger
{
    typedef QMap<int, QVector<ZZListener*>> mmap;
public:
    static ListenerManger* Instance();
    void notify(int message);
    void registerMessage(int message, ZZListener* listener);
private:
    ListenerManger() {};
    ~ListenerManger() {};
    static ListenerManger* m_listenerManger;
    QMap<int, QVector<ZZListener*>> m_messageToLister;
    QMutex m_mapMutex;
};

#endif // ZZLISTENER_H
