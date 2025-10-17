//
// Created by LCH on 24. 10. 13.
//

#include "Config.h"
#include <exception>
#include <fstream>
#include "Logger.h"
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

Config::Config(std::string path) : filePath(std::move(path))
{
    try
    {
        std::ifstream file(filePath, std::ifstream::binary);
        if (file.is_open())
        {
            root = nlohmann::json::parse(file);
            file.close();
        }
    }
    catch (std::exception& e)
    {
        Logger::GetInstance().Error("Config Construct " + std::string(e.what()));
    }
}

void Config::UpdateJson() const
{
    std::string tempFilePath = filePath + ".tmp";

    // 임시 파일에 먼저 기록
    {
        std::ofstream file(tempFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Can't open temp config : " + tempFilePath);
        }
        file << root.dump(4) << std::endl;
        file.close();
    }

#if defined(_WIN32)
    if (!MoveFileExA(tempFilePath.c_str(), filePath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        throw std::runtime_error("overwrite org file error");
    }
#else
    if (std::rename(tempFilePath.c_str(), filePath.c_str()) != 0) {
        throw std::runtime_error("overwrite org file error");
    }
#endif
}


std::string Config::GetSerialPort(const std::string& key) const
{
    try
    {
        return root.at(key).at("serialPort");
    } catch (const std::exception& e)
    {
        //TODO : 사실 여기서 exception 을 리턴해야 하는 것이 아닌지?
        Logger::GetInstance().Error("GetSerialPort Failed : " + key + ", " + std::string(e.what()));
        return "";
    }
}

std::string Config::GetModel(const std::string& key) const
{
    try
    {
        return root.at(key).at("model");
    } catch (const std::exception& e)
    {
        //TODO : 사실 여기서 exception 을 리턴해야 하는 것이 아닌지?
        Logger::GetInstance().Error("GetModel Failed : " + key + ", " + std::string(e.what()));
        return "";
    }
}

std::string Config::GetStringValue(const std::string& key, const std::string& atkey) const
{
    try
    {
        return root.at(key).at(atkey);
    } catch (const std::exception& e)
    {
        //TODO : 사실 여기서 exception 을 리턴해야 하는 것이 아닌지?
        Logger::GetInstance().Error("GetStringValue Failed : " + key + ", at : " + atkey + ", " + std::string(e.what()));
        return "";
    }
}

