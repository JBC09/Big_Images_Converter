//
// Created by chanbin on 25. 10. 15..
//

#ifndef THREADPOOL_CONSOLEMANAGER_H
#define THREADPOOL_CONSOLEMANAGER_H
#include "../../ImageCoreProcessing/include/ConverterCore.h"
#include "../../Util/include/Common.h"


class ConsoleManager
{
public:
    ConsoleManager(const std::shared_ptr<ConverterCore> &_ConverterCore) noexcept;

    ~ConsoleManager() = default;

    int ShowSelectMenu() const;

    static void ByeBye() noexcept;

    static void ShowCurrentProcess();

private:
    int InputConvertNumber() const;

    std::weak_ptr<ConverterCore> m_ConverterCore;
};


#endif //THREADPOOL_CONSOLEMANAGER_H
