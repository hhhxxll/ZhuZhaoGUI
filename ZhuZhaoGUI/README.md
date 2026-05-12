<div align="center">

# ZhuZhaoGUI

**基于光度立体法的三维表面重建桌面应用**

![Qt](https://img.shields.io/badge/Qt-6.9.3-41CD52?style=flat&logo=qt&logoColor=white)
![OpenCV](https://img.shields.io/badge/OpenCV-4.6.0-5C3EE8?style=flat&logo=opencv&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat&logo=cplusplus&logoColor=white)
![MSVC](https://img.shields.io/badge/MSVC-2022_x64-0078D4?style=flat&logo=visualstudio&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D4?style=flat&logo=windows&logoColor=white)

通过多张不同光照方向下的物体图像，恢复出物体表面的 **高度图**、**梯度图** 和 **反照率图**。

</div>

---

## 算法原理

光度立体法（Photometric Stereo）通过 4 张不同光源角度拍摄的灰度图像，利用光源方向矩阵求解每个像素的三维信息：

```
输入: 4 张灰度图 + 每张图的 Slant/Tilt 光源角度
          |
          v
构建光源方向矩阵 L (4x3)
          |
          v
逐像素求解 I = L * (k * n)  -->  法向量 n + 反照率 k
          |
          v
法向量 --> 梯度 (p, q) --> 傅里叶积分 --> 高度场
          |
          v
输出: 高度图 | 梯度图 | 反照率图
```

| 输出 | 说明 |
|------|------|
| **高度图 (Height Field)** | 物体表面的三维高度信息 |
| **梯度图 (Gradient)** | 表面法向量的梯度分布 |
| **反照率图 (Albedo)** | 物体表面的固有反射率（纹理信息） |

## 项目结构

```
ZhuZhao/
├── PhotometricStereo/              # 后端算法库 (C++ / OpenCV)
│   ├── CMakeLists.txt              # CMake 构建配置
│   ├── PhotometricStereo.cpp/h     # 核心算法实现
│   ├── ExampleMain.cpp             # 独立测试程序
│   └── images/                     # 测试图片集
│
└── ZhuZhaoGUI/                     # 前端界面 (Qt 6)
    ├── main.cpp                    # 程序入口
    ├── MainWindow.cpp/h            # 主窗口 (监听者模式)
    ├── ZZConfigWidget/             # 参数配置面板
    │   ├── ZZConfigWidget.cpp/h    # 配置界面
    │   ├── ZZOneParamWidget.cpp/h  # 单参数输入控件
    │   ├── ZZProcessThread.cpp/h   # 后台算法线程
    │   └── ImageConvert.h          # cv::Mat <-> QImage 转换
    ├── ZZLogWidget/                # 日志输出面板
    │   ├── ZZLogWidget.cpp/h       # 日志显示控件
    │   └── ZZLogMessage.cpp/h      # 日志单例 (线程安全)
    ├── ZZThumWidget/               # 图片缩略图列表
    │   ├── HThumbnailList.cpp/h    # 横向缩略图列表
    │   ├── VThumbnailList.cpp/h    # 纵向缩略图列表
    │   └── LitImgItemWidget.cpp/h  # 缩略图项控件
    ├── ZZViewWidget/               # 图像显示视图
    │   ├── CustomGraphicsView.cpp/h# 自定义视图 (缩放/拖拽)
    │   ├── CustomImageItem.cpp/h   # 图像元素
    │   └── CustomImageView.cpp/h   # 图像视图容器
    └── ZZListener.cpp/h            # 观察者-监听者模式
```

## 架构设计

```
┌─────────────────────────────────────────────────────┐
│                    MainWindow                        │
│  ┌───────────┐  ┌───────────┐  ┌─────────────────┐  │
│  │ Config    │  │ Log       │  │ ImageView       │  │
│  │ Widget    │  │ Widget    │  │ ┌─────────────┐ │  │
│  │           │  │           │  │ │ GraphicsView│ │  │
│  │ ┌───────┐ │  │ (单例日志) │  │ │ (缩放/拖拽) │ │  │
│  │ │Process│ │  │           │  │ └─────────────┘ │  │
│  │ │Thread │ │  └───────────┘  │ ┌─────────────┐ │  │
│  │ │(后台) │ │                  │ │ ThumbnailList│ │  │
│  │ └───────┘ │  Observer 模式   │ │ (横/纵列表) │ │  │
│  └───────────┘  ┌───────────┐  │ └─────────────┘ │  │
│                 │ Listener  │  └─────────────────┘  │
│                 │ Manager   │                        │
│                 └───────────┘                        │
└─────────────────────────────────────────────────────┘
```

### 设计模式

| 模式 | 应用位置 | 说明 |
|------|----------|------|
| **单例模式** | `ZZLogMessage` | 全局唯一的日志管理器 |
| **单例模式** | `ListenerManger` | 全局唯一的事件管理器 |
| **观察者-监听者** | `ListenerManger` + `ZZListener` | 事件驱动的组件通信 |
| **后台线程** | `ZZProcessThread` | 算法执行不阻塞 UI |

### 线程安全机制

| 组件 | 保护方式 |
|------|----------|
| `ZZLogMessage::Instance()` | `static QMutex` + 双重检查锁定 |
| `outputMessage()` | `static QMutex` 防止多线程并发写日志 |
| `ZZProcessThread` | `QMutex m_mutex` 保护跨线程共享数据 |
| `MainWindow` 运行控制 | `isRunning()` 检查防止线程复用竞态 |

## 环境依赖

| 依赖 | 版本 | 说明 |
|------|------|------|
| Qt | 6.9.3 | GUI 框架 |
| OpenCV | 4.6.0 | 图像处理 |
| MSVC | 2022 x64 | 编译器 |
| CMake | >= 3.22 | 构建系统 (算法库) |
| C++ | 17 | 语言标准 |

## 构建步骤

### 1. 编译算法库 (PhotometricStereoDLL)

```bash
cd PhotometricStereo
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

产物输出到 `bin/` 目录：`PhotometricStereoDLL.dll`、`PhotometricStereoDLL.lib`

### 2. 编译前端界面 (ZhuZhaoGUI)

使用 Qt Creator 打开 `ZhuZhaoGUI/ZhuZhaoGUI.pro`，或通过命令行：

```bash
cd ZhuZhaoGUI
qmake ZhuZhaoGUI.pro
nmake
```

产物输出到 `bin/` 目录：`ZhuZhaoGUI.exe`

### 3. 运行

确保 `bin/` 目录下包含以下文件后，运行 `ZhuZhaoGUI.exe`：

```
bin/
├── ZhuZhaoGUI.exe
├── PhotometricStereoDLL.dll
└── opencv_world460.dll
```

## 界面功能

| 功能 | 说明 |
|------|------|
| **图片加载** | 导入 4 张不同光照的图像 |
| **参数配置** | 设置每张图的 Slant/Tilt 光源角度 |
| **图像处理** | 一键执行光度立体算法 |
| **结果查看** | 显示高度图、梯度图、反照率图 |
| **缩略图** | 横向/纵向图片列表快速切换 |
| **图像缩放** | 鼠标滚轮缩放，双击自适应，拖拽平移 |
| **日志系统** | 实时显示处理状态，自动保存到本地文件 |
| **中文支持** | 内置中文翻译包 |

## 更新日志

### v1.1.0 (2026-05-12)

- 修复 `outputMessage` 带锁发射信号的死锁风险，将信号发射移出锁作用域
- 重构 `ZZLogMessage` 单例为 Meyers 单例（C++11 Magic Statics），移除冗余的 DCLP 锁
- 为 `ListenerManger` 添加 `QMutex` 保护，采用拷贝后分发策略避免持锁回调死锁
- 新增 `ListenerManger::unregisterAll` 注销机制，防止监听器销毁后收到回调导致崩溃
- 修复 `MainWindow` 线程复用竞态，添加 `isRunning()` 防护
- 优化 `ZZProcessThread` 函数参数为 `const` 引用
- 完善代码注释和项目文档

### v1.0.0

- 初始版本发布
- 光度立体算法集成
- Qt 6 图形界面
- 观察者-监听者模式事件系统

## 许可证

本项目仅供学习交流使用。
