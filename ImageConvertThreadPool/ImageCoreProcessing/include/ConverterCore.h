//
// Created by chanbin on 25. 10. 15..
//

#ifndef THREADPOOL_CONVERTERCORE_H
#define THREADPOOL_CONVERTERCORE_H

#include <queue>
#include <vector>
#include <string>
#include <filesystem>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <atomic>

namespace fs = std::filesystem;

enum class ConvertType
{
    PNG_TO_JPG = 1,
    JPG_TO_PNG,
    BMP_TO_PNG,
};
extern std::atomic<int> g_currentReadFileCount;
extern std::atomic<int> g_totalReadFileCount;
extern std::atomic<bool> g_isProcessing;

class ConverterCore
{
public:
    ConverterCore() noexcept;
    ~ConverterCore() = default;
    ConvertType selectConvertType;
    std::mutex m_ConvertMutex;
    std::condition_variable m_ConvertCond;

    [[nodiscard]] std::vector<std::pair<std::string, std::string>> GetConvertibleImageList() const;
    bool ConvertImage(ConvertType typpaire = ConvertType::PNG_TO_JPG);

private:
    bool loadImage = false;

    std::queue<fs::path> m_ConvertImageQueue;
    std::vector<std::pair<std::string, std::string> > m_ConvertibleImageList;
    void InputImageListPush(const fs::path& inputImagePath);
    void OutputImageDischarge(const fs::path& outputImagePath);
    void ConvertProcess();
    static void AddTextWaterMark(cv::Mat& img, const std::string& text);

};


#endif //THREADPOOL_CONVERTERCORE_H
