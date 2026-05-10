# 指定 CMake 最低版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

# 设置项目名称
PROJECT(PhotometricStereo)
add_compile_options(/utf-8)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 定义 binary 和 library 路径
set(ZZ_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "zhuzhao root")
set(ZZ_DEBUG_BUILD_DIR "${ZZ_ROOT}/../bin" CACHE PATH "Debug Build Dir")

# 设置运行时输出目录（exe 和 dll）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ZZ_DEBUG_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ZZ_DEBUG_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ZZ_DEBUG_BUILD_DIR})

# 设置库文件输出目录（lib）
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ZZ_DEBUG_BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ZZ_DEBUG_BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ZZ_DEBUG_BUILD_DIR})

# 设置静态库输出目录（.lib）
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ZZ_DEBUG_BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ZZ_DEBUG_BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ZZ_DEBUG_BUILD_DIR})

# S.1寻找第三方库
set(OpenCV_DIR "D:/CV/opencv/build/x64/vc15/lib")
FIND_PACKAGE(OpenCV REQUIRED)

# ========== 4. 创建算法动态库 ==========

# 将 PhotometricStereo.cpp 和 PhotometricStereo.h 两个文件打包，
# 存放到一个名叫 DLL_SRCS 的“文件名单”里。
FILE(GLOB 
    DLL_SRCS
    PhotometricStereo.cpp 
    PhotometricStereo.h
)

# 读取上面的 DLL_SRCS 名单，编译生成一个名叫 PhotometricStereoDLL 的动态链接库（.dll）。
# SHARED 关键字就是明确告诉 CMake：“我要生成的是 .dll，不是 .exe”。
add_library(PhotometricStereoDLL SHARED ${DLL_SRCS})

# 往 C++ 代码里强行塞入一个名为 ALGO_EXPORT 的暗号（宏定义）。
# 作用是告诉系统：“我这个库里的函数是准许被别人调用的”。
add_compile_definitions(ALGO_EXPORT)

# 给刚才生成的 PhotometricStereoDLL 库，插上 OpenCV 的底层支持。
TARGET_LINK_LIBRARIES(PhotometricStereoDLL ${OpenCV_LIBS})


# ========== 5. 创建示例程序 ==========

# 把 ExampleMain.cpp 放进一个名叫 EXAMPLE_SRCS 的“文件名单”里。
FILE(GLOB
    EXAMPLE_SRCS
    ExampleMain.cpp 
)

# 读取 EXAMPLE_SRCS 名单，编译生成一个名叫 ExampleMain 的可执行程序（.exe）。
add_executable(ExampleMain ${EXAMPLE_SRCS})

# 给生成的 ExampleMain.exe 程序，同时插上 OpenCV 和 PhotometricStereoDLL 的支持。
# 因为这个测试程序既需要用到图像处理（OpenCV），也需要测试咱们自己写的算法库（PhotometricStereoDLL）。
TARGET_LINK_LIBRARIES(ExampleMain ${OpenCV_LIBS} PhotometricStereoDLL)