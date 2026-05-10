#ifndef ZZLOGMESSAGE_H
#define ZZLOGMESSAGE_H

#include <QObject>
#include <QDebug>

//日志输出宏
//FUNCTION当前函数名,LINE当前行,
#define QDEBUG(message)     qDebug()<<" [FUNCTION]: "<<__FUNCTION__<<" [LINE]: "<<__LINE__<<" [LOG]: " << message;
#define QWARNING(message)   qWarning()<<" [FUNCTION]: "<<__FUNCTION__<<" [LINE]: "<<__LINE__<<" [LOG]: " << message;
#define QCRITICAL(message)  qCritical()<<" [FUNCTION]: "<<__FUNCTION__<<" [LINE]: "<<__LINE__<<" [LOG]: " << message;
#define QFATAL(message)     qFatal()<<" [FUNCTION]: "<<__FUNCTION__<<" [LINE]: "<<__LINE__<<" [LOG]: " << message;

class ZZLogMessage : public QObject
{
    Q_OBJECT
public:
    static ZZLogMessage* Instance();
    void installMessageHandler();
    void uninstallMessageHandler();
    QString logPath();
    QString logName();

private:
    explicit ZZLogMessage(QObject *parent = nullptr);
    ~ZZLogMessage();
    static ZZLogMessage* m_pLogInstance;

signals:
    void sigDebugStrData(const QString &);
    void sigDebugHtmlData(const QString &);
};

#endif // ZZLOGMESSAGE_H
