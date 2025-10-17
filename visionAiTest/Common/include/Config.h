//
// Created by LCH on 24. 10. 13.
//

#ifndef CLONIXCASINOSERVICE_CONFIG_H
#define CLONIXCASINOSERVICE_CONFIG_H

#include <string>
#include "json/json.hpp"

class Config
{
public:
    explicit Config(std::string  path);
    virtual ~Config() = default;

private:
    std::string filePath;

protected:
    nlohmann::json root;

    void UpdateJson() const;

public:
    [[nodiscard]] std::string GetSerialPort(const std::string& key) const;
    [[nodiscard]] std::string GetModel(const std::string& key) const;
    [[nodiscard]] std::string GetStringValue(const std::string& key, const std::string& atkey) const;
};

#endif //CLONIXCASINOSERVICE_CONFIG_H
