#pragma once

#include "json/json.hpp"
#include "Config.h"

class ClonixCasinoConfig : public Config
{
public:
    explicit ClonixCasinoConfig();

public:
    void SetVidPid(const std::string& _vid, const std::string& _pid);
};
