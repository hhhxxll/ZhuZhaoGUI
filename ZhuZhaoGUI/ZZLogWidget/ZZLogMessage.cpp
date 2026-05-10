#include "ZZLogMessage.h"
#include <QMutex>           // 互斥锁，防止多线程同时写日志导致文件混乱
#include <QDateTime>        // 获取当前系统时间，给日志打时间戳
#include <QCoreApplication> // Qt 应用核心类，获取程序运行路径等信息
#include <QFile>            // 文件读写类，用于将日志内容写入磁盘文件
#include <QDir>             // 目录操作类，用于创建日志文件夹
#include <QTextStream>      // 文本流，用于格式化写入日志文字内容

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    QString text;
    QString MessageColor = "red";
    // 根据传进来的不同日志类型，进行分类处理
    switch(type)
    {
    case QtDebugMsg:// 如果是普通的调试信息
    {
        text = QString("[Debug]");
        MessageColor = "darkgray";
        break;
    }
    case QtWarningMsg:// 如果是警告信息
    {
        text = QString("[Warning]");
        MessageColor = "darkorange";
        break;
    }
    case QtCriticalMsg:// 如果是严重报错信息
    {
        text = QString("[Error]");
        MessageColor = "red";
        break;
    }
    default:// 如果不属于上面任何一种
    {
        text = QString("[Default]");
        MessageColor = "red";
        break;
    }
    }
    //获取单例
    ZZLogMessage *instance = ZZLogMessage::Instance();
    // 获取当前系统的准确时间，格式设置为 年-月-日 时:分:秒
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("[%1]")
                               .arg(current_date_time);

    // 将上面准备好的内容，拼凑成一句完整的纯文本日志
    QString message = QString("%1%2%3")
            .arg(current_date)
            .arg(text)
            .arg(msg);

    QString messageHtml = QString("<font color=%1>" + message + "</font>").arg(MessageColor);

    //将调试信息写入文件
    QFile file(instance->logPath() + instance->logName());
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message <<"\r\n";
    file.flush();
    file.close();
    //将处理好的调试信息发送出去
    emit instance->sigDebugStrData(message);
    //将处理成 html 的调试信息发送出去
    emit instance->sigDebugHtmlData(messageHtml);
    //检查文件是否达到了指定大小
    if(file.size() < 1024*1024)
    {
        return;
    }
    //log达到了限制值则将名字更改，防止文件越来越大
    for(int loop = 1; loop < 100; ++loop)
    {
        QString fileName = QString("%1/log_%2.txt").arg(instance->logPath()).arg(loop);
        QFile logfile(fileName);
        //检查文件夹里有没有这个名字
        if(logfile.size() < 4)//小于4 字节
        {
            file.rename(fileName);
            return;
        }
    }
}

ZZLogMessage* ZZLogMessage::m_pLogInstance = nullptr;
ZZLogMessage* ZZLogMessage::Instance()
{
    if(!m_pLogInstance)
    {
        static QMutex muter;
        QMutexLocker clocker(&muter);

        if(!m_pLogInstance)
        {
            m_pLogInstance = new ZZLogMessage();
        }
    }
    return m_pLogInstance;
}

void ZZLogMessage::installMessageHandler()
{
    qInstallMessageHandler(outputMessage);
}

void ZZLogMessage::uninstallMessageHandler()
{
    qInstallMessageHandler(nullptr);
}

QString ZZLogMessage::logPath()
{
    QString current_date_file_name = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QDir dir(QString("log/%1").arg(current_date_file_name));//日志文件夹路径
    if(!dir.exists())
    {
        dir.mkpath("./");
    }
    return dir.path() + "/" ;
}

QString ZZLogMessage::logName()
{
    return "log.txt";
}

ZZLogMessage::ZZLogMessage(QObject *parent) : QObject(parent)
{

}

ZZLogMessage::~ZZLogMessage()
{

}