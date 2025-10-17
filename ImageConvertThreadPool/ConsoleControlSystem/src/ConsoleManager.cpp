//
// Created by chanbin on 25. 10. 15..
//

#include "../include/ConsoleManager.h"
#include <cstdlib>
#include <thread>
#include <iostream>


ConsoleManager::ConsoleManager(const std::shared_ptr<ConverterCore> &_ConverterCore)
    noexcept : m_ConverterCore(std::weak_ptr<ConverterCore>(_ConverterCore))
{
}

// User is selectable menu
int ConsoleManager::ShowSelectMenu() const
{
    std::system("cls");

    if (auto core = m_ConverterCore.lock())
    {
        std::vector<std::pair<std::string, std::string> > convertibleList = core->GetConvertibleImageList();

        Common::print("=== Image Converter ===\n");
        Common::print("Available Conversions:\n");

        for (size_t i = 0; i < convertibleList.size(); ++i)
        {
            Common::print(i + 1, ". ", convertibleList[i].first, " -> ", convertibleList[i].second, "\n");
        }
        Common::print("4. Exit Program\n");


        Common::print("=====================\n");
        Common::print("Select number: ");

        return InputConvertNumber();
    }

    // Error
    Common::print("[ConvertCore] Life Cycle Error, Reboot Please\n");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // 종료 코드 Return 4
    return 4;
}

int ConsoleManager::InputConvertNumber() const
{
    int inputNumber = 0;
    std::cin >> inputNumber;

    // 조건이 맞지 않았을 때~
    if (std::cin.fail() || !(inputNumber >= 1 && inputNumber <= 4))
    {
        std::system("cls");

        // 숫자가 아닌 입력이거나 해당 메뉴 범위가 아닐 때~
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        Common::print("Invalid input! Please enter a number between 1 and 4.\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // 재귀 시도
        return ShowSelectMenu();
    }

    // 정상 입력 리턴
    return inputNumber;
}


void ConsoleManager::ByeBye() noexcept
{
    std::system("cls");
    Common::print("Bye Bye!!");
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void ConsoleManager::ShowCurrentProcess()
{
    while (g_isProcessing)
    {
        Common::Logger("Show Current Process");
        int current = g_currentReadFileCount.load();
        int total = g_totalReadFileCount.load();


        if (current == 0 || total == 0)
        {
            Common::Logger(
                "Progress skipped -> current: " + std::to_string(current) +
                ", total: " + std::to_string(total)
            );
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (current == total)
        {
            break;
        }

        double progress = (static_cast<double>(current) / total) * 100.0;

        int barWidth = 50;
        int pos = static_cast<int>(barWidth * progress / 100.0);

        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i)
            std::cout << (i < pos ? "#" : " ");
        std::cout << "] " << std::setw(6) << std::fixed << std::setprecision(2)
                << progress << "%" << std::flush;
        if (current >= total) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << std::endl;
}
