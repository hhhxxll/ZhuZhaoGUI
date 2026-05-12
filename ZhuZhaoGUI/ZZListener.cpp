#include "ZZListener.h"


ListenerManger* ListenerManger::m_listenerManger = new ListenerManger();

ListenerManger* ListenerManger::Instance()
{
    return m_listenerManger;
}

void ListenerManger::notify(int message)
{
    QVector<ZZListener*> listenersToNotify;

    {
        QMutexLocker locker(&m_mapMutex);
        mmap::Iterator iter = m_messageToLister.find(message);
        if (iter != m_messageToLister.end())
        {
            listenersToNotify = iter.value();
        }
    }

    // 锁释放后再执行回调，避免 RespondMessage 中触发 registerMessage 导致死锁
    for (int i = 0; i < listenersToNotify.size(); ++i)
    {
        listenersToNotify[i]->RespondMessage(message);
    }
}

void ListenerManger::registerMessage(int message, ZZListener* listener)
{
    QMutexLocker locker(&m_mapMutex);
    auto Register = [&](int singleMessage)->void {
        mmap::iterator iter = m_messageToLister.find(singleMessage);
        if (iter == m_messageToLister.end())
        {
            QVector<ZZListener*> listeners;
            listeners.push_back(listener);
            m_messageToLister[singleMessage] = listeners;
        }
        else
        {
            iter.value().push_back(listener);
        }
    };

    if ((message & MESSAGE::ZHUZHAO_UPDATE_SRCIMAGE) == MESSAGE::ZHUZHAO_UPDATE_SRCIMAGE)
    {
        Register(MESSAGE::ZHUZHAO_UPDATE_SRCIMAGE);
    }
    if ((message & MESSAGE::ZHUZHAO_UPDATE_RESULTI) == MESSAGE::ZHUZHAO_UPDATE_RESULTI)
    {
        Register(MESSAGE::ZHUZHAO_UPDATE_RESULTI);
    }
    if ((message & MESSAGE::ZHUZHAO_RESET) == MESSAGE::ZHUZHAO_RESET)
    {
        Register(MESSAGE::ZHUZHAO_RESET);
    }
    if ((message & MESSAGE::ZHUZHAO_RUNONCE) == MESSAGE::ZHUZHAO_RUNONCE)
    {
        Register(MESSAGE::ZHUZHAO_RUNONCE);
    }
}
ZZListener::ZZListener()
{
    // 留空即可
}

void ListenerManger::unregisterAll(ZZListener* listener)
{
    QMutexLocker locker(&m_mapMutex);
    for (mmap::iterator iter = m_messageToLister.begin(); iter != m_messageToLister.end(); ++iter)
    {
        iter.value().removeAll(listener);
    }
}