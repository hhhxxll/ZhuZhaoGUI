#ifndef ZZONEPARAMWIDGET_H
#define ZZONEPARAMWIDGET_H

#include <QWidget>  // 导入基础窗口组件
#include <QLabel>   // 导入文本标签组件
#include <QDoubleSpinBox>   // 导入带小数点的数字微调输入框组件
#include <QPushButton>  // 导入按钮组件

class ZZOneParamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ZZOneParamWidget(QString paramName, QWidget *parent = nullptr);

    QImage GetQimage();                  // 获取组件里存储的图片
    void SetSlantAngle(float fSlant);    // 设置倾斜角（Slant）的数值
    float GetSlantAngle();               // 获取当前输入的倾斜角数值
    void SetTiltAngle(float fTilt);      // 设置俯仰角（Tilt）的数值
    float GetTiltAngle();                // 获取当前输入的俯仰角数值
    void Reset();                        // 重置组件状态

protected:
    bool InitWidget();  // 初始化界面组件的方法

protected slots:
    void OnSigLoadImageBtnClicked(bool clicked);// 当加载图片按钮被点击时执行的函数

private:
    QString m_strParamName;        // 存储这个组件的名字（字符串类型）
    QLabel* m_pTitleLabel;         // 显示标题文字的标签指针
    QLabel* m_pSlantsLabel;        // 显示“Slant:”字样的标签指针
    QDoubleSpinBox* m_pSlantsSpin; // 输入 Slant 角度的小数微调框指针
    QLabel* m_pTiltsLabel;         // 显示“Tilt:”字样的标签指针
    QDoubleSpinBox* m_pTiltsSpin;  // 输入 Tilt 角度的小数微调框指针
    QPushButton* m_pLoadImageBtn;  // “加载图片”按钮指针
    QImage m_qImage;               // 存储实际图片数据的对象
};

#endif // ZZONEPARAMWIDGET_H
