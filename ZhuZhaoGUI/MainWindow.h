#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ZZListener.h"

QT_BEGIN_NAMESPACE
namespace  Ui { class MainWindow; }
QT_END_NAMESPACE

class ZZConfigWidget;
class ZZLogWidget;
class CustomImageView;
class HThumbnailList;
class VThumbnailList;
class ZZProcessThread;
class MainWindow : public QMainWindow, ZZListener
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void RespondMessage(int message) override;

protected:
    bool InitWidget();

protected slots:
    void OnProcessThreadFinished();

private:
    ZZConfigWidget* m_pConfigWidget;
    ZZLogWidget* m_pLogWidget;
    CustomImageView* m_pImageView;
    HThumbnailList* m_pHThumList;
    VThumbnailList* m_pVThumList;
    ZZProcessThread* m_pRunProcess;
};
#endif // MAINWINDOW_H
