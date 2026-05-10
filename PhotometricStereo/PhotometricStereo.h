// 1. 头文件防卫式声明：防止这个文件被别人重复引用导致报错
#ifndef _ZZ_PHOTEMETRIC_STEREO_H_
#define _ZZ_PHOTEMETRIC_STEREO_H_

// 2. C和C++兼容开关（作为基础底座，防止以后引入C语言代码时名字冲突）
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C 
#endif

// 3. 动态库导出宏：根据“是否有暗号”来决定是发货还是进货
#ifdef ALGO_EXPORT
#define ZZ_API __declspec(dllexport) // 算法库自己编译时：标记为允许外部调用
#else
#define ZZ_API __declspec(dllimport) // 主界面拿来用时：标记为引入外部函数
#endif

// 4. 引入 OpenCV 视觉包和 C++ 标准库
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// 5. 将算法收纳到 ZhuZhao 这个命名空间（专属文件夹）里
namespace ZhuZhao
{
    // 这就是整个算法库唯一对外开放的“核心大门”
    // ZZ_API 决定了这个函数能不能被打包进 .dll 提供给外面的主界面
    uint32_t ZZ_API PhotometricStereo(
        const vector<Mat>& srcImages, // 输入：用户传进来的多张原始图片
        Mat& HeightField,             // 输出：算完后交出去的高度图
        Mat& Gradient,                // 输出：算完后交出去的梯度图
        Mat& Albedo,                  // 输出：算完后交出去的反射率图（纹理图）
        int ImageCount,               // 输入：图片的数量
        vector<float> Slants,         // 输入：每张图对应的光源倾斜角度
        vector<float> Tilts           // 输入：每张图对应的光源偏转角度
    );
}

#endif // !_ZZ_PHOTEMETRIC_STEREO_H_