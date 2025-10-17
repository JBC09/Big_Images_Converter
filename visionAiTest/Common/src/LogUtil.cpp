#include "LogUtil.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <vector>

// 파일명("yyyymmdd_log.txt")에서 앞 8자리를 날짜 정보로 추출하여 LogDate 구조체로 반환합니다.
LogDate LogUtil::extractLogDateFromFilename(const std::string &filename) {
    // 파일명에서 마지막 '/' 이후 부분만 추출
    std::string::size_type pos = filename.find_last_of('/');
    std::string shortFilename = (pos == std::string::npos) ? filename : filename.substr(pos + 1);

    if (shortFilename.size() < 8)
    {
        throw std::runtime_error("파일명이 너무 짧습니다: " + shortFilename);
    }
    // 파일명의 처음 8문자가 날짜 정보라고 가정 (예: "20250414")
    std::string dateStr = shortFilename.substr(0, 8);
    if (dateStr.size() != 8)
    {
        throw std::runtime_error("날짜 형식이 올바르지 않습니다: " + dateStr);
    }
    LogDate logDate{};
    logDate.year = std::stoi(dateStr.substr(0, 4));
    logDate.month = std::stoi(dateStr.substr(4, 2));
    logDate.day = std::stoi(dateStr.substr(6, 2));
    return logDate;
}

std::chrono::system_clock::time_point LogUtil::parseLogTime(const std::string &logTimeStr,
                                                              const LogDate &logDate) {
    std::string ts = logTimeStr;
    // 앞뒤의 대괄호 제거
    if (!ts.empty() && ts.front() == '[')
        ts.erase(0, 1);
    if (!ts.empty() && ts.back() == ']')
        ts.pop_back();

    // 콜론(:)을 기준으로 분리 → 시, 분, 초, 밀리초
    std::vector<std::string> tokens;
    std::istringstream iss(ts);
    std::string token;
    while (std::getline(iss, token, ':')) {
        tokens.push_back(token);
    }
    if (tokens.size() != 4) {
        throw std::runtime_error("로그 시간 형식 오류: " + logTimeStr);
    }
    int hour = std::stoi(tokens[0]);
    int minute = std::stoi(tokens[1]);
    int second = std::stoi(tokens[2]);
    int millisecond = std::stoi(tokens[3]);

    std::tm tmTime = {};
    tmTime.tm_year = logDate.year - 1900; // tm_year는 1900년 기준
    tmTime.tm_mon  = logDate.month - 1;    // tm_mon은 0~11
    tmTime.tm_mday = logDate.day;
    tmTime.tm_hour = hour;
    tmTime.tm_min  = minute;
    tmTime.tm_sec  = second;

    std::time_t tt = std::mktime(&tmTime);
    if (tt == -1) {
        throw std::runtime_error("시간 변환 실패");
    }
    auto baseTimePoint = std::chrono::system_clock::from_time_t(tt);
    return baseTimePoint + std::chrono::milliseconds(millisecond);
}

bool LogUtil::isNewLogEntry(const std::string &line) {
    if (line.empty()) return false;
    if (line.front() != '[') return false;
    if (line.size() < 4) return false;
    return std::isdigit(line[1]) && std::isdigit(line[2]);
}

void LogUtil::extractLogFromFileMultiline(const std::string &filename,
                                            const std::chrono::system_clock::time_point &lowerBound,
                                            const std::chrono::system_clock::time_point &upperBound,
                                            bool lowerBoundCheck,
                                            bool upperBoundCheck,
                                            std::ofstream &outFile) {
    // 파일명에서 LogDate 추출 (예: "20250414_log.txt"에서 "20250414")
    LogDate logDate = extractLogDateFromFilename(filename);

    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "파일 열기 실패: " << filename << std::endl;
        return;
    }
    std::string currentEntry;  // 하나의 로그 메시지를 누적
    std::string line;
    while (std::getline(inFile, line)) {
        if (isNewLogEntry(line)) {
            // 이전에 누적한 로그 메시지가 있다면 처리
            if (!currentEntry.empty()) {
                std::istringstream entryStream(currentEntry);
                std::string timeToken;
                entryStream >> timeToken;  // 첫 토큰이 시간 정보여야 함.
                try {
                    auto entryTime = parseLogTime(timeToken, logDate);
                    bool include = true;
                    if (lowerBoundCheck && entryTime < lowerBound)
                        include = false;
                    if (upperBoundCheck && entryTime > upperBound)
                        include = false;
                    if (include) {
                        outFile << currentEntry << "\n";
                    }
                } catch (const std::exception &ex) {
                    std::cerr << "로그 파싱 오류: " << ex.what() << std::endl;
                }
            }
            // 새 로그 메시지 시작: currentEntry 재설정
            currentEntry = line;
        } else {
            // 새 로그 메시지의 시작 표시가 없는 경우, 현재 로그 메시지의 연장으로 취급
            if (!currentEntry.empty())
                currentEntry += "\n" + line;
            else
                currentEntry = line;
        }
    }
    // 마지막 누적 메시지 처리
    if (!currentEntry.empty()) {
        std::istringstream entryStream(currentEntry);
        std::string timeToken;
        entryStream >> timeToken;
        try {
            auto entryTime = parseLogTime(timeToken, logDate);
            bool include = true;
            if (lowerBoundCheck && entryTime < lowerBound)
                include = false;
            if (upperBoundCheck && entryTime > upperBound)
                include = false;
            if (include) {
                outFile << currentEntry << "\n";
            }
        } catch (const std::exception &ex) {
            std::cerr << "로그 파싱 오류: " << ex.what() << std::endl;
        }
    }
    inFile.close();
}

void LogUtil::extractLog(const std::chrono::system_clock::time_point& startTime)
{
    auto now = std::chrono::system_clock::now();

    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::time_t startT = std::chrono::system_clock::to_time_t(startTime);
    // time_t를 std::tm 구조체로 변환 (로컬 시간 기준)
    std::tm* start_tm = std::localtime(&startT);
    std::tm* now_tm = std::localtime(&nowTime);

    auto fileNameFromTimePoint = [](const std::tm& tm)-> std::string
    {
        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << (tm.tm_year + 1900)
            << std::setw(2) << std::setfill('0') << (tm.tm_mon + 1)
            << std::setw(2) << std::setfill('0') << tm.tm_mday
            << "_Log.txt";
        return oss.str();
    };

    const auto startFileName = "D:/ClonixCasinoService/Logs/" + fileNameFromTimePoint(*start_tm);
    const auto endFileName = "D:/ClonixCasinoService/Logs/" + fileNameFromTimePoint(*now_tm);

    // 년, 월, 일, 시, 분, 초 추출 (주의: tm_year는 1900년 기준, tm_mon은 0~11)
    const int year  = now_tm->tm_year + 1900;
    const int month = now_tm->tm_mon + 1;
    const int day   = now_tm->tm_mday;
    const int hour  = now_tm->tm_hour;
    const int minute = now_tm->tm_min;
    const int second = now_tm->tm_sec;

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year
        << std::setw(2) << std::setfill('0') << month
        << std::setw(2) << std::setfill('0') << day
        << "_"
        << std::setw(2) << std::setfill('0') << hour
        << std::setw(2) << std::setfill('0') << minute
        << std::setw(2) << std::setfill('0') << second
        << "_Fail_Log.txt";

    std::ofstream outFile("D:/ClonixCasinoService/Logs/" + oss.str());

    if (startFileName == endFileName)
    {
        extractLogFromFileMultiline(endFileName, startTime, now, true, true, outFile);
    }
    else
    {
        extractLogFromFileMultiline(startFileName, startTime, now, true, false, outFile);

        extractLogFromFileMultiline(endFileName, startTime, now, false, true, outFile);
    }

    outFile.close();
}
