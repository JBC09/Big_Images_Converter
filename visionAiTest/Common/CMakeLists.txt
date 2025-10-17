project(Common)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_sources(${PROJECT_NAME} PRIVATE
        src/Logger.cpp
        src/SerialPort.cpp
        src/Config.cpp
        src/ClonixCasinoConfig.cpp
        src/LogUtil.cpp
)