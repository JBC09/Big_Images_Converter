cmake_minimum_required(VERSION 3.30)
project(ImageConvertThreadPool
    VERSION 0.1
    DESCRIPTION "ImageConvertThreadPool"
    LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

add_executable(ImageConvertThreadPool main.cpp
        Util/src/Common.cpp
        Util/include/Common.h
        ConsoleControlSystem/src/ConsoleManager.cpp
        ConsoleControlSystem/include/ConsoleManager.h
        ImageCoreProcessing/src/ConverterCore.cpp
        ImageCoreProcessing/include/ConverterCore.h
        ThreadPool/src/ThreadPool.cpp
        ThreadPool/include/ThreadPool.h)


include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
