# ZhuZhaoGUI - 光度立体视觉 GUI 应用

基于 **光度立体法（Photometric Stereo）** 的三维表面重建桌面应用，通过多张不同光照方向下的物体图像，恢复出物体表面的高度图、梯度图和反照率图。

## 项目结构

```
ZhuZhaoGUI/
├── PhotometricStereo/          # 后端算法库（C++ / OpenCV）
│   ├── CMakeLists.txt          # CMake 构建配置
│   ├── PhotometricStereo.cpp/h # 核心算法实现
│   ├── ExampleMain.cpp         # 独立测试程序
│   └── images/                 # 测试图片集
│
└── ZhuZhaoGUI/                 # 前端界面（Qt 6）
    ├── main.cpp                # 程序入口
    ├── MainWindow.cpp/h        # 主窗口
    ├── ZZConfigWidget/         # 参数配置面板
    ├── ZZLogWidget/            # 日志输出面板
    ├── ZZThumWidget/           # 图片缩略图列表
    └── ZZViewWidget/           # 图像显示视图
```

## 算法原理

光度立体法通过 4 张不同光源角度拍摄的灰度图像，利用光源方向矩阵求解每个像素的：

- **表面法向量（Normal）** — 物体表面的朝向
- **反照率（Albedo）** — 物体表面的固有反射率（纹理信息）
- **高度图（Height Field）** — 通过梯度积分恢复的表面三维高度

核心流程：
1. 根据 Slant/Tilt 角度构建光源方向矩阵 `L`
2. 对每个像素求解 `I = L * (k * n)`，得到法向量 `n` 和反照率 `k`
3. 由法向量计算梯度 `(p, q)`，通过傅里叶变换积分恢复高度场

## 环境依赖

| 依赖 | 版本 |
|------|------|
| OpenCV | 4.6.0 |
| Qt | 6.9.3 |
| MSVC | 2022 x64 |
| CMake | >= 3.22 |
| C++ 标准 | C++17 |

## 构建步骤

### 1. 编译算法库（PhotometricStereoDLL）

```bash
cd PhotometricStereo
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

产物输出到 `bin/` 目录：`PhotometricStereoDLL.dll`、`PhotometricStereoDLL.lib`

### 2. 编译前端界面（ZhuZhaoGUI）

使用 Qt Creator 打开 `ZhuZhaoGUI/ZhuZhaoGUI.pro`，或通过命令行：

```bash
cd ZhuZhaoGUI
qmake ZhuZhaoGUI.pro
nmake
```

产物输出到 `bin/` 目录：`ZhuZhaoGUI.exe`

### 3. 运行

确保 `bin/` 目录下包含以下文件后，运行 `ZhuZhaoGUI.exe`：

- `PhotometricStereoDLL.dll`
- OpenCV 动态库（`opencv_world460.dll`）

## 界面功能

- **图片加载** — 导入 4 张不同光照的图像
- **参数配置** — 设置每张图的 Slant/Tilt 光源角度
- **图像处理** — 一键执行光度立体算法
- **结果查看** — 显示高度图、梯度图、反照率图
- **缩略图** — 横向/纵向图片列表快速切换
- **日志系统** — 实时显示处理状态和调试信息
- **中文支持** — 内置中文翻译包
