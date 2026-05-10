# ZhuZhaoGUI 项目面试深度解析

---

## 一、项目整体业务流程（面试开场口述版）

> **一句话定义：** ZhuZhaoGUI 是一个基于 **光度立体法（Photometric Stereo）** 的三维表面重建桌面工具，用户输入多张不同光照角度拍摄的灰度图，程序通过算法反演出物体表面的 **高度图、梯度图、反照率图**。

**核心业务流程（5 步）：**

```
[1] 启动 → 加载中文翻译包 → 初始化日志系统（单例 + qInstallMessageHandler）
         ↓
[2] 用户在左侧面板配置 4 组参数（每组：一张图片 + Slant角度 + Tilt角度）
    - 点击 "load" 按钮 → QFileDialog 选择单张图片
    - 输入 Slant（倾斜角）和 Tilt（偏转角），描述该图片拍摄时的光源方向
         ↓
[3] 点击 "RunOnce" 按钮 → 触发观察者模式通知 → 启动后台线程 ZZProcessThread
         ↓
[4] 后台线程执行光度立体法核心算法：
    - QImage → cv::Mat 格式转换
    - 根据 Slant/Tilt 计算 4 个光源方向向量 → 构建光源矩阵 L
    - SVD 求逆 → 逐像素计算法向量 N 和反照率 ρ
    - FFT 频域积分 → 从梯度场恢复高度场 Z
    - cv::Mat → QImage 格式转换
         ↓
[5] 线程结束 → 结果图（高度图/梯度图/反照率图）显示在底部横向缩略图列表
    → 用户点击缩略图 → 中央视图区展示大图（支持滚轮缩放、拖拽平移、像素级 RGB 取色）
```

**口述版本（面试时这样说）：**

> "这个项目是一个光度立体法的可视化工具。用户在左侧面板分别加载 4 张不同光源角度拍摄的物体照片，同时输入每张照片对应的光源 Slant 和 Tilt 角度。点击运行后，程序在后台线程中执行光度立体法算法，通过光源矩阵求逆得到每个像素的法向量和反照率，再通过 FFT 频域积分从梯度场恢复出高度图。最终结果以缩略图形式展示在底部，用户点击可查看大图，支持缩放和像素取色。"

---

## 二、核心技术架构 + 模块分工

### 2.1 整体分层架构

```
┌─────────────────────────────────────────────────────────┐
│                    MainWindow (主窗口)                     │
│         QMainWindow + ZZListener（同时继承两者）            │
├──────────┬──────────┬──────────┬──────────┬──────────────┤
│ ZZConfig │ ZZLog    │ Custom   │ HThumb   │ VThumbnail   │
│ Widget   │ Widget   │ Image    │ nailList │ List         │
│ 配置面板  │ 日志面板  │ View     │ 横向缩略图│ 纵向缩略图   │
│          │          │ 图像视图  │ (结果图)  │ (输入图)     │
├──────────┴──────────┴──────────┴──────────┴──────────────┤
│               ZZProcessThread (后台算法线程)                │
│               继承 QThread，调用 PhotometricStereo DLL     │
├─────────────────────────────────────────────────────────┤
│          PhotometricStereoDLL (算法动态库)                  │
│          OpenCV + FFT + SVD → 高度场/梯度场/反照率          │
└─────────────────────────────────────────────────────────┘
```

### 2.2 模块职责与依赖关系

| 模块                              | 核心职责                      | 依赖                           | 设计模式                 |
| ------------------------------- | ------------------------- | ---------------------------- | -------------------- |
| `MainWindow`                    | 界面组装、消息分发、协调各子模块          | 所有子模块                        | 观察者（ZZListener）      |
| `ZZConfigWidget`                | 管理 4 组参数输入（图片+角度）         | ZZOneParamWidget, ZZListener | 组合模式                 |
| `ZZOneParamWidget`              | 单组参数：图片加载 + Slant/Tilt 输入 | ZZListener                   | —                    |
| `ZZLogWidget`                   | 日志显示面板                    | ZZLogMessage                 | 观察者（信号槽）             |
| `ZZLogMessage`                  | 全局日志系统（单例）                | 无                            | 单例 + 消息劫持            |
| `CustomImageView`               | 图像显示容器                    | CustomGraphicsView           | 委托模式                 |
| `CustomGraphicsView`            | 图像缩放/拖拽/取色/背景绘制           | CustomImageItem              | MVC（Scene/View/Item） |
| `CustomImageItem`               | 鼠标悬浮时发射 RGB 坐标信号          | 无                            | 观察者（信号槽）             |
| `HThumbnailList`                | 横向缩略图列表（显示结果图）            | LitImgItemWidget             | —                    |
| `VThumbnailList`                | 纵向缩略图列表（显示输入图）            | LitImgItemWidget             | —                    |
| `LitImgItemWidget`              | 单个缩略图项（带红/黑边框绘制）          | 无                            | 自定义绘制                |
| `ZZProcessThread`               | 后台执行光度立体法算法               | PhotometricStereoDLL, OpenCV | 生产者-消费者              |
| `ZZListener` / `ListenerManger` | 自定义观察者消息系统                | 无                            | 观察者 + 单例             |

### 2.3 数据流架构

```
用户操作                    消息系统                      数据处理
─────────                  ─────────                    ─────────
load按钮 → QFileDialog    ──→ ZHUZHAO_UPDATE_SRCIMAGE  ──→ VThumbnailList 刷新输入图
              ↓                     ↓
       ZZOneParamWidget          MainWindow::RespondMessage
       存储QImage+角度            ↓
                            GetPhotometricStereoParams
                            提取(QImage[], Slant[], Tilt[])
                                     ↓
RunOnce按钮 ─────────────→ ZHUZHAO_RUNONCE
                                     ↓
                            ZZProcessThread::start()
                                     ↓
                            run() {
                              QImage→cv::Mat
                              PhotometricStereo()  ← DLL
                              cv::Mat→QImage
                            }
                                     ↓
                            finished信号 ──→ OnProcessThreadFinished
                                     ↓
                            HThumbnailList 刷新结果图
                                     ↓
点击缩略图 → SigSelectImageChanged ──→ CustomImageView::OnSendImage
                                     ↓
                            CustomGraphicsView::SetImage
                            (缩放/居中/显示)
```

### 2.4 UI 布局结构

```
MainWindow (QMainWindow)
└── pCenterWidget (QWidget)
    └── pMainSplitter (QSplitter 水平可拖动)
        ├── 左侧: pLeftSplitter (QSplitter 垂直可拖动)
        │   ├── ZZConfigWidget (配置面板，含4组参数+Reset/RunOnce按钮)
        │   └── ZZLogWidget (日志面板，含QTextBrowser+Clear/Helper按钮)
        └── 右侧: pRightWidget (QWidget)
            ├── VThumbnailList (左侧纵向缩略图，宽120px)
            └── pViewLayout (QVBoxLayout)
                ├── CustomImageView (中央图像视图，可缩放/拖拽)
                ├── HThumbnailList (底部横向缩略图，高120px)
                └── addStretch() (底部弹簧)
```

---

## 三、面试高频重难点

### 3.1 核心算法源码逻辑

#### 3.1.1 光度立体法核心（PhotometricStereo.cpp）

**算法原理：** 对于朗伯体表面，像素亮度 I = ρ · (N · L)，其中 ρ 是反照率，N 是法向量，L 是光源方向。4 张图、4 个已知光源方向，可以反解出每个像素的 N 和 ρ。

```cpp
// 核心计算流程（逐像素）：
cv::Mat n = LightsInv * cv::Mat(I);  // L⁻¹ × I = ρN（4×4 × 4×1 = 4×1）
float kd = sqrt(n.dot(n));            // kd = |ρN| = ρ（因为|N|=1）
if (kd > 0) { n = n / kd; }          // 归一化得到法向量 N
AlbedoMap.at<float>(...) = kd / 255;  // 反照率
Pgrads = n[0] / n[2];                 // p = ∂z/∂x（x方向梯度）
Qgrads = n[1] / n[2];                 // q = ∂z/∂y（y方向梯度）
```

**高度场恢复（globalHeights 函数）：** 通过 FFT 频域积分从梯度场 (p, q) 恢复高度场 Z，使用的是 **Frankot-Chellappa 算法**：

```cpp
// 频域中：Z(u,v) = (j·u·P + j·v·Q) / ((1+λ)(u²+v²) + μ(u²+v²)²)
// 其中 λ=1.0, μ=1.0 是正则化参数，防止分母为零
float d = (1.0f + lambda) * uv + mu * pow(uv, 2);
Z[0] = (u * P[1] + v * Q[1]) / d;  // 实部
Z[1] = (-u * P[0] - v * Q[0]) / d; // 虚部
// 最后 IDFT 回到空间域得到高度图
```

#### 3.1.2 观察者模式实现（ZZListener.h/cpp）

```cpp
// 自定义消息枚举（位掩码设计，支持组合订阅）
enum MESSAGE {
    ZHUZHAO_UPDATE_SRCIMAGE = 0x00000001,  // 001
    ZHUZHAO_UPDATE_RESULTI  = 0x00000002,  // 010
    ZHUZHAO_RESET           = 0x00000003,  // 011（注意：这是前两个的按位或）
    ZHUZHAO_RUNONCE         = 0x00000004,  // 100
};
```

**注册机制：** 用位运算拆分组合消息，逐一注册到 `QMap<int, QVector<ZZListener*>>` 中。

**通知机制：** `notify(message)` 在 map 中查找对应 message，遍历所有注册的 listener 调用 `RespondMessage()`。

#### 3.1.3 QImage ↔ cv::Mat 转换（ImageConvert.h）

支持三种格式：`CV_8UC1`（灰度）、`CV_8UC3`（RGB）、`CV_8UC4`（ARGB），通过 `clone` 参数控制是否共享内存，`rb_swap` 控制 RGB↔BGR 通道交换。

### 3.2 易踩坑 Bug

#### Bug 1：MESSAGE 枚举值设计缺陷

```cpp
enum MESSAGE {
    ZHUZHAO_UPDATE_SRCIMAGE = 0x00000001,  // 1
    ZHUZHAO_UPDATE_RESULTI  = 0x00000002,  // 2
    ZHUZHAO_RESET           = 0x00000003,  // 3 = 1|2 ← 问题！
    ZHUZHAO_RUNONCE         = 0x00000004,  // 4
};
```

**问题：** `ZHUZHAO_RESET = 3 = 0x01 | 0x02`，这意味着当发送 RESET 消息时：

- `RespondMessage` 中 `(message & ZHUZHAO_UPDATE_SRCIMAGE) == ZHUZHAO_UPDATE_SRCIMAGE` 为 **true**
- `RespondMessage` 中 `(message & ZHUZHAO_UPDATE_RESULTI) == ZHUZHAO_UPDATE_RESULTI` 也为 **true**

RESET 消息会意外触发 UPDATE_SRCIMAGE 和 UPDATE_RESULTI 的处理逻辑。

**正确设计应该是独立的位：**

```cpp
ZHUZHAO_RESET = 0x00000008,  // 第4位，与其他互不干扰
```

#### Bug 2：ZHUZHAO_UPDATE_RESULTI 消息处理为空

```cpp
// MainWindow.cpp:132
if((message & MESSAGE::ZHUZHAO_UPDATE_RESULTI) == MESSAGE::ZHUZHAO_UPDATE_RESULTI)
{
    // 空实现！
}
```

这个消息虽然注册了监听，但处理函数为空，属于未完成功能。

#### Bug 3：ListenerManger 单例内存泄漏

```cpp
// ZZListener.cpp:4
ListenerManger* ListenerManger::m_listenerManger = new ListenerManger();
```

堆上分配的单例永远不会被释放。虽然程序退出时 OS 会回收，但严格来说是内存泄漏。且 `Instance()` 不是线程安全的（没有加锁）。

#### Bug 4：LogMessage 文件大小检查时机错误

```cpp
// ZZLogMessage.cpp:60-65
QFile file(instance->logPath() + instance->logName());
file.open(QIODevice::WriteOnly | QIODevice::Append);
QTextStream text_stream(&file);
text_stream << message << "\r\n";
file.flush();
file.close();          // ← 文件已关闭
// ...
if(file.size() < 1024*1024)  // ← 检查的是关闭后的状态，可能不准确
{
    return;
}
```

应该在 `file.close()` 之前检查文件大小，或者在写入后重新 `open` 再检查。

#### Bug 5：CustomImageItem 的 w/h 成员未初始化

```cpp
// CustomImageItem.h
public:
    int w;  // 未初始化，随机值
    int h;  // 未初始化，随机值
```

在 `SetImage` 中才赋值，但在此之前如果触发 `hoverMoveEvent`，会使用随机值显示在界面上。

#### Bug 6：QImage 传引用的信号槽问题

```cpp
// HThumbnailList.h
signals:
    void SigSelectImageChanged(QImage& qImage);  // 引用类型信号
```

Qt 信号槽传递引用类型在跨线程时会导致 **未定义行为**（Qt 会尝试拷贝但引用语义不明确）。虽然当前都是同线程调用，但这是潜在隐患。应该改为值传递 `QImage` 或指针 `QImage*`。

#### Bug 7：PhotometricStereo 中的合法像素判断失效

```cpp
// PhotometricStereo.cpp:110-111
for (int i = 0; i < NUM_IMGS; i++) {
    legit *= modelImages[i].at<uchar>(Point(x, y)) >= 0;  // uchar 永远 >= 0！
}
```

`uchar`（unsigned char）的值范围是 0~255，`>= 0` 永远为 true，所以 `legit` 永远为 1。这段"边界保护"代码完全失效。应该是想判断是否 `> 0` 或者检查某个阈值。

### 3.3 性能瓶颈及优化方案

#### 瓶颈 1：逐像素循环计算法向量（O(W×H×4)）

```cpp
// PhotometricStereo.cpp:92-125
for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
        // 每个像素都做一次矩阵乘法 LightsInv * I
        cv::Mat n = LightsInv * cv::Mat(I);
    }
}
```

**问题：** 逐像素创建 `cv::Mat` 对象，有大量内存分配/释放开销；无法利用 SIMD 并行。

**优化方案：**

- **矩阵化：** 将所有像素的亮度向量 I 组成一个 `(4, W*H)` 的大矩阵，一次性 `LightsInv × I_all` 完成所有计算
- **OpenMP 并行化：** `#pragma omp parallel for` 加速双层循环
- **GPU 加速：** 使用 `cv::cuda::GpuMat` 将矩阵运算放到 GPU

#### 瓶颈 2：FFT 高度场恢复中的逐像素访问

```cpp
// globalHeights 函数中
for (int i = 0; i < Pgrads.rows; i++) {
    for (int j = 0; j < Pgrads.cols; j++) {
        Z.at<cv::Vec2f>(i, j)[0] = ...;  // 逐像素访问，缓存不友好
    }
}
```

**优化方案：** 使用 OpenCV 的 `cv::mulSpectrums` 等向量化操作替代逐像素循环。

#### 瓶颈 3：QImage → cv::Mat 转换时的内存拷贝

```cpp
// ZZProcessThread.cpp:48-49
QImage qsrcImage = srcImages.at(i).convertToFormat(QImage::Format_Grayscale8);
cv::Mat srcImage = QImage2cvMat(qsrcImage);  // clone=true，深拷贝
```

每张图都做了一次格式转换 + 一次深拷贝。如果图片分辨率高（如 4K），4 张图的拷贝开销显著。

**优化方案：** 提前在 UI 层统一转为灰度格式存储，避免在线程中重复转换；或使用 `clone=false` 共享内存（需确保 QImage 生命周期覆盖线程执行期间）。

#### 瓶颈 4：缩略图渲染效率

每次 `addImage` 都创建新的 `LitImgItemWidget` + `QListWidgetItem`，大量图片时会有 UI 卡顿。

**优化方案：** 使用 `QListView` + `QAbstractListModel` 的 Model/View 架构，实现懒加载和虚拟化渲染。

---

## 四、面试官追问刁钻问题 + 标准满分回答

---

### Q1：为什么选择自定义观察者模式（ZZListener）而不是直接用 Qt 的信号槽？

**满分回答：**

> 这个项目中确实存在两套通信机制并存的情况。Qt 信号槽用于同模块或父子组件间的直接通信（如 `SigSelectImageChanged`、`finished` 信号），而自定义观察者模式用于 **跨模块的广播式通信**。
> 
> 两者的核心区别在于 **耦合度**：
> 
> - 信号槽需要发送者持有接收者的引用（或通过 connect 绑定），是 **点对点** 的
> - 观察者模式通过 ListenerManger 中介，发送者完全不知道谁在接收，是 **一对多广播**
> 
> 比如 `RunOnce` 按钮被点击时，它不知道也不关心谁需要响应这个事件，只需要 `notify(ZHUZHAO_RUNONCE)` 即可。如果用信号槽，ZZConfigWidget 就需要持有 MainWindow 的引用，破坏了模块独立性。
> 
> 但这个实现有一个问题：`registerMessage` 用位运算拆分注册，但枚举值设计有冲突（`ZHUZHAO_RESET = 3 = 1|2`），会导致消息误触发。更好的做法是用独立的 2 的幂次方作为枚举值，或者直接用 Qt 的 `QEvent` 系统来做自定义事件分发。

---

### Q2：PhotometricStereo 算法中为什么用 FFT 而不是逐像素积分来恢复高度场？

**满分回答：**

> 从梯度场 (p, q) 恢复高度场 z，本质上是求解泊松方程：∂²z/∂x² + ∂²z/∂y² = ∂p/∂x + ∂q/∂y。
> 
> **空间域逐积分的问题：**
> 
> - 路径依赖：从不同起点、不同路径积分得到的结果不一致
> - 误差累积：每一步积分都会累积前面的误差，距离起点越远误差越大
> - 噪声敏感：局部噪声会影响全局结果
> 
> **FFT 频域积分的优势：**
> 
> - 泊松方程在频域中变为代数方程：Z(u,v) = (j·u·P + j·v·Q) / D(u,v)，直接求解
> - 全局最优：一次 FFT + 逐点除法 + IFFT，得到的是全局最小二乘解
> - 可加入正则化项（代码中的 λ 和 μ）控制平滑度，抑制噪声
> 
> 这就是经典的 **Frankot-Chellappa 算法**，是光度立体法中高度恢复的标准做法。计算复杂度 O(N·log·N)，优于空间域的 O(N²)。

---

### Q3：`ZZProcessThread` 中用 `QMutex` 保护数据，但 `run()` 中又对输入数据做了拷贝，为什么不直接在锁内处理？

**满分回答：**

> 这是一个典型的 **最小化锁持有时间** 的设计。
> 
> ```cpp
> void ZZProcessThread::run() {
>     // 1. 短暂加锁，拷贝输入数据到局部变量
>     {
>         QMutexLocker locker(&m_mutex);
>         srcImages = m_srcImages;  // 拷贝
>         srcSlants = m_Slants;
>     }
>     // 2. 锁已释放，用局部副本做长时间计算
>     PhotometricStereo(...);
>     // 3. 计算完毕，短暂加锁写回结果
>     {
>         QMutexLocker locker(&m_mutex);
>         m_dstImages = ...;
>     }
> }
> ```
> 
> 如果在锁内执行整个算法（可能耗时数秒），那么 `SetPhotometricStereoParams()` 会被阻塞，导致 UI 线程卡死（因为 Set 是在主线程调用的）。通过拷贝到局部变量后立即释放锁，主线程可以随时修改输入参数而不被阻塞。
> 
> 这是 **Copy-In/Copy-Out** 模式，是多线程编程中减少锁竞争的标准手法。

---

### Q4：这个项目的观察者模式实现有什么问题？如何改进？

**满分回答：**

> 有三个核心问题：
> 
> **1. 消息枚举值冲突（最严重）：**
> `ZHUZHAO_RESET = 3 = ZHUZHAO_UPDATE_SRCIMAGE | ZHUZHAO_UPDATE_RESULTI`，导致 RESET 消息会触发 SRCIMAGE 和 RESULTI 的处理逻辑。应该改为 `0x00000008` 等独立位。
> 
> **2. 单例不是线程安全的：**
> 
> ```cpp
> ListenerManger* ListenerManger::Instance() {
>     return m_listenerManger;  // 没有锁保护
> }
> ```
> 
> 虽然 `m_listenerManger` 在全局初始化时就分配了（饿汉模式），所以 `Instance()` 本身是安全的。但 `registerMessage()` 和 `notify()` 操作 `QMap` 不是线程安全的，如果多线程同时注册/通知会崩溃。
> 
> **3. 没有反注册机制：**
> 如果某个 ZZListener 对象被销毁了，但 ListenerManger 中还持有它的指针，下次 notify 就是 **悬空指针调用**，直接崩溃。应该提供 `unregisterMessage()` 方法，并在 ZZListener 析构时自动反注册。
> 
> **改进方案：** 用 `QPointer<ZZListener>` 替代裸指针，或者干脆用 Qt 的 `QEvent` 系统 + `QCoreApplication::sendEvent/postEvent` 来实现自定义事件分发，天然支持线程安全和对象生命周期管理。

---

### Q5：QImage 和 cv::Mat 的转换中，`clone=false` 共享内存模式有什么风险？

**满分回答：**

> `clone=false` 意味着 QImage 和 cv::Mat 指向同一块内存数据，风险在于：
> 
> **1. 生命周期问题：**
> 如果 QImage 先被销毁（比如局部变量离开作用域），其内部缓冲区被释放，cv::Mat 就变成了 **悬空指针**，访问时会崩溃。反过来 cv::Mat 先销毁也一样。
> 
> **2. 通道顺序问题：**
> OpenCV 用 BGR，QImage 用 RGB。当 `rb_swap=true` 且 `clone=false` 时：
> 
> ```cpp
> cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);  // 直接修改了共享内存！
> ```
> 
> 这会 **反向修改原始 QImage 的像素数据**，导致 QImage 显示的颜色错乱。
> 
> **3. 写时复制冲突：**
> QImage 有隐式共享（implicit sharing）机制，当 QImage 被拷贝时，底层数据是共享的，直到某一方写入时才触发深拷贝。如果 cv::Mat 直接修改了共享内存，QImage 的隐式共享机制不会感知到，可能导致读到脏数据。
> 
> **结论：** 在大多数场景下应该使用 `clone=true`（默认值），除非你能严格保证：(a) 生命周期一致 (b) 只读访问 (c) 通道顺序已正确处理。

---

### Q6：如果要支持处理 100 张高分辨率图片，这个架构需要怎么改？

**满分回答：**

> 当前架构有三个瓶颈会阻塞大规模处理：
> 
> **1. UI 线程阻塞：**
> `GetPhotometricStereoParams()` 在主线程遍历所有参数组件提取 QImage，100 张 4K 图的拷贝会卡 UI。改为：在 ZZOneParamWidget 中存储图片路径而非 QImage，运行时由后台线程按需加载。
> 
> **2. 内存爆增：**
> 100 张 4K 灰度图约 100 × 3840×2160 × 1 = 790MB，加上 cv::Mat 副本和中间结果，峰值可能超 3GB。改为：使用 **分块处理** 或 **流式加载**，每次只加载 N 张到内存。
> 
> **3. 算法并行度不足：**
> 当前逐像素循环是纯 CPU 串行。改为：
> 
> - 矩阵化运算：将 100 张图的像素组成 `(100, W×H)` 矩阵，一次性矩阵乘法
> - GPU 加速：用 CUDA 或 OpenCL 将矩阵运算放到 GPU
> - 多线程分块：将图像分成 4~8 个 tile，每个线程处理一个 tile
> 
> **4. UI 渲染优化：**
> 100 个缩略图项的 QListWidget 会很卡。改为 QListView + QAbstractListModel 的 Model/View 架构，配合 QThreadPool 实现异步缩略图生成。
> 
> **5. 进度反馈：**
> 长时间运算需要进度条。可以在 ZZProcessThread 中 emit `progress(int percent)` 信号，主线程用 QProgressBar 显示。

---

### Q7：为什么用 QThread 而不是 QtConcurrent 或线程池？

**满分回答：**

> 对于这个项目，QThread 是合适的选择，原因如下：
> 
> **QThread 适用场景：** 需要精细控制线程生命周期、长时间运行的单一任务、需要与 UI 线程交互（通过信号槽）。
> 
> **QtConcurrent 适用场景：** 简单的"发射后不管"任务，比如 `QtConcurrent::run(func)` 一行代码搞定。
> 
> **QThreadPool 适用场景：** 大量短小任务需要并发处理，比如批量图片缩略图生成。
> 
> 这里选择 QThread 的理由：
> 
> 1. 光度立体法是一个 **单一的长时间任务**，不是大量小任务
> 2. 需要在运行前通过 `SetPhotometricStereoParams()` 传入参数，运行后通过 `GetResultImage()` 取出结果，需要 **双向数据交换**，QThread 用 QMutex 保护更直观
> 3. 需要通过 `finished` 信号通知 UI 线程更新界面
> 4. 算法过程本身不适合拆分成多个并行小任务（FFT 是全局运算）
> 
> 如果改用 QtConcurrent，代码会更简洁但控制力更弱。如果未来需要支持"连续处理多组参数"的场景，可以考虑用 QThreadPool + QRunnable 实现任务队列。

---

### Q8：`LitImgItemWidget` 的 `paintEvent` 为什么不用 QLabel + QPixmap 来替代？

**满分回答：**

> 因为 `LitImgItemWidget` 需要 **动态绘制选中状态的红/黑边框**：
> 
> ```cpp
> void LitImgItemWidget::paintEvent(QPaintEvent *event) {
>     QPainter painter(this);
>     painter.drawPixmap(2, 2, width-4, height-4, tempPixmap);  // 绘制图片
>     QPen pen;
>     pen.setColor(this->hasFocus() ? Qt::red : Qt::black);     // 根据焦点变色
>     pen.setWidth(4);
>     painter.drawRect(QRect(2, 2, width-4, height-4));          // 绘制边框
> }
> ```
> 
> 如果用 QLabel + QPixmap：
> 
> - 边框颜色变化需要通过 `setStyleSheet()` 动态切换 QSS，频繁切换会有样式刷新开销
> - QLabel 的 `setPixmap()` 会自动缩放，但缩放策略（KeepAspectRatio 等）不够灵活
> - 重绘边框时 QLabel 会整体刷新，而自定义 paintEvent 可以精确控制绘制区域
> 
> 自定义 paintEvent 的优势：
> 
> 1. **精确控制**：图片留 2px 边距，边框 4px 宽，焦点变色，完全自定义
> 2. **性能**：只在 `update()` 时重绘，避免 QLabel 的样式引擎开销
> 3. **扩展性**：未来可以轻松叠加更多绘制效果（如选中高亮、加载动画等）

---

### Q9：项目中有哪些地方体现了 Qt 的内存管理机制？

**满分回答：**

> **1. 父子对象树（Object Tree）：**
> 所有 `new` 出来的 QWidget 都传入了 `this` 作为 parent：
> 
> ```cpp
> m_pConfigWidget = new ZZConfigWidget(this);
> m_pLogWidget = new ZZLogWidget(this);
> ```
> 
> 当 MainWindow 销毁时，Qt 会自动递归 delete 所有子对象，无需手动释放。
> 
> **2. 布局的所有权转移：**
> 
> ```cpp
> QSplitter* pLeftSplitter = new QSplitter(Qt::Vertical);
> pLeftSplitter->addWidget(m_pConfigWidget);  // 所有权转移到 splitter
> ```
> 
> `addWidget` 后，布局管理器接管了 widget 的内存管理。
> 
> **3. 信号槽自动断开：**
> 当 QObject 被销毁时，Qt 自动断开它参与的所有信号槽连接，防止悬空指针调用。
> 
> **4. 隐式共享（Implicit Sharing）：**
> QImage 使用写时复制：
> 
> ```cpp
> QImage a("test.png");
> QImage b = a;  // 共享数据，不拷贝
> b.setPixel(0, 0, Qt::red);  // 此时才触发深拷贝
> ```
> 
> **5. 唯一的反例 — ListenerManger：**
> 用裸 `new` 创建的单例和裸指针存储的 listener 列表，没有利用 Qt 的对象树机制，存在内存泄漏和悬空指针风险。这是需要改进的地方。

---

### Q10：如果面试官问"你从这个项目中学到了什么"，怎么回答？

**满分回答：**

> **三个层面的收获：**
> 
> **1. 架构设计层面：**
> 学会了如何用观察者模式解耦跨模块通信。但同时我也认识到，当框架本身提供了成熟的事件系统（Qt 的信号槽、QEvent）时，自定义实现需要谨慎权衡——我们的观察者模式存在枚举值冲突和悬空指针问题，这让我理解了"不要重复造轮子"的实际含义。
> 
> **2. 性能优化层面：**
> 光度立体法的逐像素计算让我深刻理解了"算法复杂度 ≠ 实际性能"——即使理论复杂度相同，矩阵化的批量操作比逐像素循环快一个数量级，因为 CPU 缓存命中率和 SIMD 利用率完全不同。
> 
> **3. 工程实践层面：**
> 
> - QImage 和 cv::Mat 的转换不是简单的格式适配，涉及内存管理、通道顺序、写时复制等陷阱
> - 多线程中"最小化锁持有时间"的 Copy-In/Copy-Out 模式是减少 UI 卡顿的关键
> - Qt 的父子对象树虽然方便，但需要理解所有权语义，否则会出现双重释放或内存泄漏
