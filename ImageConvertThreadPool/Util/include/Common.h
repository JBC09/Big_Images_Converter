//
// Created by chanbin on 25. 10. 15..
//

#ifndef THREADPOOL_COMMON_H
#define THREADPOOL_COMMON_H
#include <iostream>
#include <string_view>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <fstream>
#include <cstdlib>
#define NOMINMAX
#include <windows.h>
#include <mutex>
namespace fs = std::filesystem;

namespace Common
{
    template<typename... Args>
    void print(Args &&... args)
    {
        (std::cout << ... << args); // fold expression After C++ 17
    }

    // 디렉터리 만들고 비우는 함수 ㅋ
    inline fs::path DirectoryCreateAndClear(const std::string_view &message)
    {
        // 현재 위치 가져오기
        fs::path exeCurrentPath = fs::current_path();

        // 그 위치 옆에 폴더 만들기~
        fs::path ImagePath = exeCurrentPath / message;

        // 폴더가 존재하지 않는다면~ 새로 만들어유
        if (!fs::exists(ImagePath))
        {
            fs::create_directory(ImagePath);
            print("Input Directory created.\n");
            print("Please place the image in the input directory.\n");
        }
        // 폴더가 존재한다면 안에 내용을 삭제해유
        else
        {
            print("Input Directory already exists.\n");
            print("Delete Before Image.\n");

            for (auto &entry: fs::directory_iterator(ImagePath))
            {
                fs::remove_all(entry.path());
            }
        }

        return ImagePath;
    }

    // 로그 만드는 함수 ㅋ
    inline void Logger(const std::string_view &message)
    {
        // 로그 폴더는 삭제하지 말고 유지
        fs::path LoggerDirectory = fs::current_path() / "Log";

        if (!fs::exists(LoggerDirectory))
        {
            fs::create_directory(LoggerDirectory);
            print("Log Directory created.\n");
        }

        //  파일 이름 (날짜 기반)
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss, ss2;
        ss << std::put_time(std::localtime(&t), "(%Y-%m-%d)_ConvertLog");
        ss2 << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");

        fs::path logPath = LoggerDirectory / (ss.str() + "log.txt");

        //  파일 열기 (존재하면 append, 없으면 새로 생성)
        std::ofstream logFile(logPath, std::ios::out | std::ios::app);
        if (!logFile.is_open())
        {
            print("Failed to open log file\n");
            return;
        }

        // ✅ 로그 내용 기록
        logFile << "[ " << ss2.str() << " ]\t" << message << std::endl;
        logFile.close();
    }

    inline void OpenExplorer(const std::string &directory)
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        fs::path exeDir = fs::path(buffer).parent_path();

        fs::path targetPath = exeDir / directory;

        std::string command = "explorer.exe \"" + targetPath.string() + "\"";

        system(command.c_str());
    }
};


#endif //THREADPOOL_COMMON_H
