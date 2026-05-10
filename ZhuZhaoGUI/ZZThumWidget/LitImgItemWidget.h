#ifndef LITIMGITEMWIDGET_H
#define LITIMGITEMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class LitImgItemWidget : public QWidget
{
    Q_OBJECT
public:
    LitImgItemWidget(QWidget *parent = nullptr);
    ~LitImgItemWidget();
    bool InitWidget();

    void setImage(QImage& qImage);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QImage m_qImg;
    QString m_strName;
};

#endif // LITIMGITEMWIDGET_H
