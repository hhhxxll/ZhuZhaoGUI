#include "ZZListener.h"


ListenerManger* ListenerManger::m_listenerManger = new ListenerManger();

ListenerManger* ListenerManger::Instance()
{
    return m_listenerManger;
}

void ListenerManger::notify(int message)
{
    //在m_messageToLister字典里找message数字在哪里
    mmap::Iterator iter = m_messageToLister.find(message);
    //如果没有找到字典最后,说明这个数字存在
    if (iter != m_messageToLister.end())
    {
        QVector<ZZListener*>::iterator listener = iter.value().begin();
        //循环listener名单,只要没有执行到最后
        while (listener != iter.value().end())
        {
            (*listener)->RespondMessage(message);
            listener++;
        }
    }
    else
    {

    }
 }

void ListenerManger::registerMessage(int message, ZZListener* listener)
{
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