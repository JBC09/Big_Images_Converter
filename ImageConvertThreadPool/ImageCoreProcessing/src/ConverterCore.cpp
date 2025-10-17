//
// Created by chanbin on 25. 10. 15..
//

#include "../include/ConverterCore.h"
#include "../../Util/include/Common.h"
#include "../../ConsoleControlSystem/include/ConsoleManager.h"
#include "../../ThreadPool/include/ThreadPool.h"

std::atomic<int> g_currentReadFileCount{0};
std::atomic<int> g_totalReadFileCount{0};
std::atomic<bool> g_isProcessing{false};

ConverterCore::ConverterCore()
    noexcept : m_ConvertibleImageList({}), selectConvertType(ConvertType::PNG_TO_JPG)
{
    // 변환 가능한 확장자 쌍 등록
    m_ConvertibleImageList.emplace_back(".PNG", ".JPG");
    m_ConvertibleImageList.emplace_back(".JPG", ".PNG");
    m_ConvertibleImageList.emplace_back(".BMP", ".PNG");
}

std::vector<std::pair<std::string, std::string>> ConverterCore::GetConvertibleImageList() const
{
    return m_ConvertibleImageList;
}

bool ConverterCore::ConvertImage(ConvertType typpaire)
{
    selectConvertType = typpaire;
    auto [before, after] = m_ConvertibleImageList[static_cast<int>(selectConvertType) - 1];

    Common::Logger("Convert Image Start");
    Common::Logger("User Select ConvertType : " + before + " -> " + after);

    // 큐 초기화
    {
        std::lock_guard<std::mutex> lock(m_ConvertMutex);
        while (!m_ConvertImageQueue.empty())
            m_ConvertImageQueue.pop();
    }

    ConvertProcess();

    Common::Logger("Convert Image End");
    return true;
}

void ConverterCore::ConvertProcess()
{
    g_isProcessing = true;
    Common::Logger("ConvertProcess Start");


    // input 폴더 생성 및 초기화
    fs::path inputImagePath = Common::DirectoryCreateAndClear("input");
    Common::OpenExplorer("input");

    Common::print("When the images are ready, type any text and press Enter.\n");

    std::string input;
    std::getline(std::cin, input);
    std::getline(std::cin, input);

    // output 폴더 생성 및 초기화
    fs::path outputImagePath = Common::DirectoryCreateAndClear("output");

    // 스레드 풀 생성 (시스템 코어 수만큼)
    ThreadPool pool(std::thread::hardware_concurrency());

    g_currentReadFileCount = 0;

    // 생산자 스레드 (이미지 큐 채우기)
    pool.enqueue([this, inputImagePath]() {
        InputImageListPush(inputImagePath);
    });

    // 소비자 스레드 여러 개 실행
    int workerCount = std::max(2u, std::thread::hardware_concurrency());
    for (int i = 0; i < workerCount-1; ++i)
    {
        pool.enqueue([this, outputImagePath]() {
            OutputImageDischarge(outputImagePath);
        });
    }

    ConsoleManager::ShowCurrentProcess();

    pool.WaitAll();

    Common::OpenExplorer("output");
    Common::Logger("ConvertProcess End");
}

void ConverterCore::InputImageListPush(const fs::path &inputImagePath)
{
    loadImage = false;
    int count = 0;

    for (const auto &entry : fs::directory_iterator(inputImagePath))
        if (entry.is_regular_file())
            ++count;

    if (count == 0)
    {
        return;
    }

    g_totalReadFileCount = count;

    for (const auto &entry : fs::directory_iterator(inputImagePath))
    {
        if (!entry.is_regular_file())
            continue;

        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

        auto [before, after] = m_ConvertibleImageList[static_cast<int>(selectConvertType) - 1];

        if (ext == before)
        {
            {
                std::lock_guard<std::mutex> lock(m_ConvertMutex);
                m_ConvertImageQueue.push(entry.path());
                Common::Logger("📥 Read Image -> " + entry.path().string());
            }
            m_ConvertCond.notify_one();
        }
        else
        {
            Common::Logger(" Skip file (not matching extension) -> " + entry.path().string());
        }
    }

    // 로드 완료
    {
        std::lock_guard<std::mutex> lock(m_ConvertMutex);
        loadImage = true;
    }
    m_ConvertCond.notify_all();
}

void ConverterCore::OutputImageDischarge(const fs::path &outputImagePath)
{
    if (!fs::exists(outputImagePath))
    {
        Common::Logger(" Output folder not found: " + outputImagePath.string());
        return;
    }

    while (true)
    {
        fs::path imageItem;

        // 큐 접근 구간 (락 유지 최소화)
        {
            std::unique_lock<std::mutex> lock(m_ConvertMutex);

            m_ConvertCond.wait(lock, [this]() {
                return !m_ConvertImageQueue.empty() || (m_ConvertImageQueue.empty() && loadImage);
            });

            if (m_ConvertImageQueue.empty() && loadImage)
            {
                m_ConvertCond.notify_all();
                break;
            }

            imageItem = m_ConvertImageQueue.front();
            m_ConvertImageQueue.pop();
        }

        // --- 락 해제 후 이미지 변환 작업 시작 ---
        Common::Logger("🔹 Processing: " + imageItem.string());

        cv::Mat img;
        fs::path out;

        switch (selectConvertType)
        {
            case ConvertType::PNG_TO_JPG:
                img = cv::imread(imageItem.string(), cv::IMREAD_UNCHANGED);
                if (img.empty())
                {
                    Common::Logger(" Failed to read PNG image");
                    continue;
                }
                if (img.channels() == 4)
                    cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
                out = outputImagePath / imageItem.filename();
                out.replace_extension(".jpg");
                Common::Logger("🟡 Converting PNG → JPG: " + out.string());
                break;

            case ConvertType::JPG_TO_PNG:
                img = cv::imread(imageItem.string(), cv::IMREAD_COLOR);
                if (img.empty())
                {
                    Common::Logger(" Failed to read JPG image");
                    continue;
                }
                out = outputImagePath / imageItem.filename();
                out.replace_extension(".png");
                Common::Logger(" Converting JPG → PNG: " + out.string());
                break;

            case ConvertType::BMP_TO_PNG:
                img = cv::imread(imageItem.string(), cv::IMREAD_COLOR);
                if (img.empty())
                {
                    Common::Logger(" Failed to read BMP image");
                    continue;
                }
                out = outputImagePath / imageItem.filename();
                out.replace_extension(".png");
                Common::Logger(" Converting BMP → PNG: " + out.string());
                break;

            default:
                Common::Logger(" Invalid convert type selected");
                continue;
        }

        if (img.empty())
        {
            Common::Logger(" Image is empty: " + imageItem.string());
            continue;
        }

        AddTextWaterMark(img, "© CREATE BY CHANBIN");

        bool ok = cv::imwrite(out.string(), img);
        if (!ok)
            Common::Logger(" Failed to write image: " + out.string());
        else
            Common::Logger(" Successfully saved: " + out.string());

        ++g_currentReadFileCount;
    }

    Common::Logger(" Thread finished. Converted files: " + std::to_string(g_currentReadFileCount));
}

void ConverterCore::AddTextWaterMark(cv::Mat &img, const std::string &text)
{
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;

    int baseline = 0;
    cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
    cv::Point pos(img.cols - textSize.width - 20, img.rows - textSize.height - 20);

    // 반투명 텍스트 효과
    cv::putText(img, text, pos + cv::Point(2, 2), fontFace, fontScale, cv::Scalar(0, 0, 0),
                thickness + 1, cv::LINE_AA);
    cv::putText(img, text, pos, fontFace, fontScale, cv::Scalar(255, 255, 255),
                thickness, cv::LINE_AA);
}
