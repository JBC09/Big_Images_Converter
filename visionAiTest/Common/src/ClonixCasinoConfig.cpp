#include "ClonixCasinoConfig.h"

#include <iostream>
#include <fstream>



ClonixCasinoConfig::ClonixCasinoConfig() : Config(
        R"(D:\ClonixCasinoService\ClonixCasinoConfig.json)")
{
}

void ClonixCasinoConfig::SetVidPid(const std::string& _vid, const std::string& _pid)
{
        root["titoreader"]["vid"] = _vid;
        root["titoreader"]["pid"] = _pid;

        UpdateJson();
}

