#添加依赖的QT，主要包含core gui widgets
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#声明C++语言规范版本
CONFIG += c++17
DEFINES += QT_DEPRECATED_WARNINGS

#添加源文件和头文件
SOURCES += \
    ZZConfigWidget/ZZConfigWidget.cpp \
    ZZConfigWidget/ZZOneParamWidget.cpp \
    ZZConfigWidget/ZZProcessThread.cpp \
    ZZListener.cpp \
    ZZLogWidget/ZZLogMessage.cpp \
    ZZLogWidget/ZZLogWidget.cpp \
    ZZThumWidget/HThumbnailList.cpp \
    ZZThumWidget/LitImgItemWidget.cpp \
    ZZThumWidget/VThumbnailList.cpp \
    ZZViewWidget/CustomGraphicsView.cpp \
    ZZViewWidget/CustomImageItem.cpp \
    ZZViewWidget/CustomImageView.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h \
    ZZConfigWidget/ImageConvert.h \
    ZZConfigWidget/ZZConfigWidget.h \
    ZZConfigWidget/ZZOneParamWidget.h \
    ZZConfigWidget/ZZProcessThread.h \
    ZZListener.h \
    ZZLogWidget/ZZLogMessage.h \
    ZZLogWidget/ZZLogWidget.h \
    ZZThumWidget/HThumbnailList.h \
    ZZThumWidget/LitImgItemWidget.h \
    ZZThumWidget/VThumbnailList.h \
    ZZViewWidget/CustomGraphicsView.h \
    ZZViewWidget/CustomImageItem.h \
    ZZViewWidget/CustomImageView.h

RESOURCES += \
    ZhuZhaoGuiRes.qrc \
    ZhuzhaoGuiRes.qrc

#配置opencv库 (使用正确的 -L 和 -l 分离语法)
INCLUDEPATH += D:/CV/opencv/build/include

CONFIG(debug, debug|release) {
    LIBS += -LD:/CV/opencv/build/x64/vc15/lib \
            -lopencv_world460d
} else {
    LIBS += -LD:/CV/opencv/build/x64/vc15/lib \
            -lopencv_world460
}

INCLUDEPATH += $$PWD/../PhotometricStereo
LIBS += -L$$PWD/../bin \
        -lPhotometricStereoDLL

#配置生成路径，将我们的结果输出产物输出到bin文件夹内，方便管理
CONFIG(debug, debug|release){
    DESTDIR = $$PWD/../bin
}else{
    DESTDIR = $$PWD/../bin
}

#设置图标
RC_ICONS = zhuzhao_logo.ico

#添加翻译文件
TRANSLATIONS += language_ch.ts \
    language_ch.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
