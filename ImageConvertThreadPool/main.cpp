#include <any>
#include <chrono>
#include "ConsoleControlSystem/include/ConsoleManager.h"
#include <thread>

int main()
{
    std::cout.setf(std::ios::unitbuf);

    // 필요한 Convert와 Console 객체 생성
    std::shared_ptr<ConverterCore> L_ConverterCore = std::make_shared<ConverterCore>();
    std::shared_ptr<ConsoleManager> L_ConsoleManager = std::make_shared<ConsoleManager>(L_ConverterCore);

    int SelectOption = 0;

    // 변환 가능한 메뉴 선택
    while (true)
    {
        SelectOption = L_ConsoleManager->ShowSelectMenu();

        // 종료 조건
        if (SelectOption == 4)
        {
            break;
        }

        L_ConverterCore->ConvertImage(static_cast<ConvertType>(SelectOption));
    }

    // 종료
    L_ConsoleManager->ByeBye();
    return 0;
}
