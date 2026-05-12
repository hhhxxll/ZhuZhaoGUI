#include "ZZLogMessage.h"
#include <QMutex>           // 互斥锁，防止多线程同时写日志导致文件混乱
#include <QDateTime>        // 获取当前系统时间，给日志打时间戳
#include <QCoreApplication> // Qt 应用核心类，获取程序运行路径等信息
#include <QFile>            // 文件读写类，用于将日志内容写入磁盘文件
#include <QDir>             // 目录操作类，用于创建日志文件夹
#include <QTextStream>      // 文本流，用于格式化写入日志文字内容

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    ZZLogMessage *instance = ZZLogMessage::Instance();

    QString text;
    QString MessageColor = "red";
    // 根据传进来的不同日志类型，进行分类处理
    switch(type)
    {
    case QtDebugMsg:
    {
        text = QString("[Debug]");
        MessageColor = "darkgray";
        break;
    }
    case QtWarningMsg:
    {
        text = QString("[Warning]");
        MessageColor = "darkorange";
        break;
    }
    case QtCriticalMsg:
    {
        text = QString("[Error]");
        MessageColor = "red";
        break;
    }
    default:
    {
        text = QString("[Default]");
        MessageColor = "red";
        break;
    }
    }

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1%2%3").arg(current_date).arg(text).arg(msg);
    QString messageHtml = QString("<font color=%1>" + message + "</font>").arg(MessageColor);

    // 仅在文件操作范围内持锁，避免带锁 emit 信号导致死锁
    {
        static QMutex mutex;
        QMutexLocker lock(&mutex);

        QFile file(instance->logPath() + instance->logName());
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();

        if(file.size() < 1024 * 1024)
        {
            // 文件未超限，锁在此释放，继续发送信号
        }
        else
        {
            // log达到了限制值则将名字更改，防止文件越来越大
            for(int loop = 1; loop < 100; ++loop)
            {
                QString fileName = QString("%1/log_%2.txt").arg(instance->logPath()).arg(loop);
                QFile logfile(fileName);
                if(logfile.size() < 4)
                {
                    file.rename(fileName);
                    break;
                }
            }
        }
    } // 锁在此释放

    // 锁释放后再发射信号，避免死锁
    emit instance->sigDebugStrData(message);
    emit instance->sigDebugHtmlData(messageHtml);
}

ZZLogMessage* ZZLogMessage::Instance()
{
    // Meyers 单例：C++11 保证 static 局部变量初始化线程安全，无需额外加锁
    static ZZLogMessage instance;
    return &instance;
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