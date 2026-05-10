#ifndef ZZCONFIGWIDGET_H
#define ZZCONFIGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>

class ZZOneParamWidget;
class ZZConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ZZConfigWidget(QWidget *parent = nullptr);

    void GetPhotometricStereoParams(QList<QImage>& srcImages,
                                    QList<float>& Slants,
                                    QList<float>& Tilts);

protected:
    bool InitWidget();

protected slots:
    void OnResetBtnClicked(bool clicked);
    void OnRunOnceBtnClicked(bool clicked);

private:
    QLabel* m_pTitleLabel;
    QList<ZZOneParamWidget*> m_ParamWidgetList;
    QPushButton* m_pResetBtn;
    QPushButton* m_pRunOnceBtn;
};

#endif // ZZCONFIGWIDGET_H
