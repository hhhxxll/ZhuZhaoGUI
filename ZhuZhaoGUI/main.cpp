#include "MainWindow.h"
#include <QStyleFactory>
#include <QApplication>
#include <QTranslator>
#include "ZZLogWidget/ZZLogMessage.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStyle *style = QStyleFactory::create("fusion");
    a.setStyle(style);

    ZZLogMessage::Instance()->installMessageHandler();
    QDEBUG("启动日志系统");

    QTranslator* pTranslator = new QTranslator();
    bool isLoaded = pTranslator->load(":/Resouce/translate/language_ch.qm");

    if (isLoaded) {
        // 如果加载成功，才安装翻译器
        a.installTranslator(pTranslator);
        QDEBUG("中文包加载成功");
    } else {
        // 如果加载失败，在控制台报警，防止无声无息地失效
        qDebug() << "警告：中文包加载失败，请检查 qrc 资源路径是否正确！";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
