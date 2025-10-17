//
// Created by Clonix on 25. 4. 15.
//

#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <chrono>
#include <string>

// 로그 파일명 "yyyymmdd_log.txt"에서 날짜 정보를 나타내기 위한 구조체
struct LogDate
{
    int year;
    int month;
    int day;
};

class LogUtil
{
    // 파일명("yyyymmdd_log.txt")에서 LogDate를 추출하는 함수
    // 파일명의 처음 8자리가 날짜 정보 (예: "20250414")라고 가정합니다.
    static LogDate extractLogDateFromFilename(const std::string& filename);

    // 대괄호 안의 "HH:MM:SS:밀리초" 문자열과 LogDate를 결합해 time_point를 생성
    static std::chrono::system_clock::time_point parseLogTime(
        const std::string& logTimeStr,
        const LogDate& logDate
    );

    // 로그 라인이 새 로그 메시지의 시작인지 판단하는 함수.
    // 로그 메시지의 시작은 '['로 시작하고 그 뒤에 숫자가 오는 형태로 판단합니다.
    static bool isNewLogEntry(const std::string& line);


    // 멀티라인 로그 메시지를 지정한 시간 범위 내에서 추출하는 함수.
    // 파일명에서 날짜 정보를 자동으로 추출하므로 별도 LogDate 전달 없이 사용합니다.
    static void extractLogFromFileMultiline(
        const std::string& filename,
        const std::chrono::system_clock::time_point& lowerBound,
        const std::chrono::system_clock::time_point& upperBound,
        bool lowerBoundCheck,
        bool upperBoundCheck,
        std::ofstream& outFile
    );

public:
    static void extractLog(const std::chrono::system_clock::time_point& startTime);
};

#endif //LOGUTIL_H
