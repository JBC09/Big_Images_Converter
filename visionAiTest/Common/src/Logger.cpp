#include "Logger.h"

#include <string>
#include <Windows.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

#ifdef _DEBUG
#define LOG(message) std::wcout << message << std::endl
#define LOGA(message) std::cout << message << std::endl
#define LOG_NOWRAP(message) std::wcout << message
#define LOG_PERR(message) perror(message)
#endif

Logger::Logger()
	: defaultFileName(L"Log.txt")
	, logLevel(ClonixLogLevel::TRACE)
{
}

Logger::~Logger()
{
	Close();
}



void Logger::SetDefaultFileName(const std::wstring& _defaultFileName)
{
	defaultFileName = _defaultFileName;
}

void Logger::SetLogLevel(const ClonixLogLevel& _logLevel)
{
	logLevel = _logLevel;
}


void Logger::Trace(const std::string& _message)
{
	if (static_cast<int>(ClonixLogLevel::TRACE) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOGA("[TRACE] " + _message);
#endif
	Write(ClonixLogLevel::TRACE, _message);
}

void Logger::Trace(const std::wstring& _message)
{
	if (static_cast<int>(ClonixLogLevel::TRACE) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOG(L"[TRACE] " + _message);
#endif
	Write(ClonixLogLevel::TRACE, _message);
}

void Logger::Debug(const std::string& _message)
{
	if (static_cast<int>(ClonixLogLevel::DBG) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOGA("[DBG] " + _message);
#endif
	Write(ClonixLogLevel::DBG, _message);
}

void Logger::Debug(const std::wstring& _message)
{
	if (static_cast<int>(ClonixLogLevel::DBG) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOG(L"[DBG] " + _message);
#endif
	Write(ClonixLogLevel::DBG, _message);
}

void Logger::Info(const std::string& _message)
{
	if (static_cast<int>(ClonixLogLevel::INFO) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOGA("[INFO] " + _message);
#endif
	Write(ClonixLogLevel::INFO, _message);
}

void Logger::Info(const std::wstring& _message)
{
	if (static_cast<int>(ClonixLogLevel::INFO) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOG(L"[INFO] " + _message);
#endif
	Write(ClonixLogLevel::INFO, _message);
}

void Logger::Warning(const std::string& _message)
{
	if (static_cast<int>(ClonixLogLevel::WARN) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOGA("[WARN] " + _message);
#endif
	Write(ClonixLogLevel::WARN, _message);
}

void Logger::Warning(const std::wstring& _message)
{
	if (static_cast<int>(ClonixLogLevel::WARN) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOG(L"[WARN] " + _message);
#endif
	Write(ClonixLogLevel::WARN, _message);
}

void Logger::Error(const std::string& _message)
{
	if (static_cast<int>(ClonixLogLevel::ERR) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOGA("[ERR] " + _message);
#endif
	Write(ClonixLogLevel::ERR, _message);
}

void Logger::Error(const std::wstring& _message)
{
	if (static_cast<int>(ClonixLogLevel::ERR) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOG(L"[ERR] " + _message);
#endif
	Write(ClonixLogLevel::ERR, _message);
}

void Logger::Fatal(const std::string& _message)
{
	if (static_cast<int>(ClonixLogLevel::FATAL) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOGA("[FATAL] " + _message);
#endif
	Write(ClonixLogLevel::FATAL, _message);
}

void Logger::Fatal(const std::wstring& _message)
{
	if (static_cast<int>(ClonixLogLevel::FATAL) < static_cast<int>(logLevel)) return;

#ifdef _DEBUG
	LOG(L"[FATAL] " + _message);
#endif
	Write(ClonixLogLevel::FATAL, _message);
}

void Logger::Close()
{
	//std::cout << "[Logger::Close] close!!!! " << std::endl;	//hhhhhhh test

	if (!os.is_open()) return;

	os.flush();
	os.close();
}


void Logger::Write(ClonixLogLevel _logLevel, const std::string& _message)
{	
	Write(_logLevel, stringToWString(_message));
}

bool GetLogPath(wchar_t* _path, int _len)
{
	wchar_t buffer[MAX_PATH] = { 0x00, };
	if (GetModuleFileNameW(NULL, buffer, MAX_PATH) == 0) return false;

	std::wstring modulePath(buffer);
	size_t lastBackslashPos = modulePath.find_last_of(L'\\');
	if (lastBackslashPos != std::wstring::npos) {
		std::wstring pathOnly = modulePath.substr(0, lastBackslashPos);

		swprintf_s(_path, _len, L"%s", pathOnly.c_str());
	}
	else {
		return false;
	}

	return true;
}

void Logger::Write(ClonixLogLevel _logLevel, const std::wstring& _message)
{
	std::lock_guard<std::mutex> lockGuard(mutex);

	const auto timePoint = std::chrono::system_clock::now();
	auto currentDate = GetDate(timePoint);

	if (!os.is_open())
	{
		date = currentDate;

		wchar_t path[MAX_PATH] = { 0x00, };
		//SYSTEM privilege returns "Windows System32" path
		//if (GetCurrentDirectory(MAX_PATH, path) == 0) return;
		if (GetLogPath(path, MAX_PATH) == false) return;

		wcscat_s(path, sizeof(path) / sizeof(wchar_t), L"/Logs/");
		if (_waccess_s(path, 0) != 0)
		{
			CreateDirectory(path, nullptr);
		}

		std::wstring file = path;
		file += date + L"_" + defaultFileName;
		os.open(file, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
	}
	else if (date.compare(currentDate) != 0)
	{
		date = currentDate;
		os.close();

		wchar_t path[MAX_PATH] = { 0x00, };
		//SYSTEM privilege returns "Windows System32" path
		//if (GetCurrentDirectory(MAX_PATH, path) == 0) return;
		if (GetLogPath(path, MAX_PATH) == false) return;

		wcscat_s(path, sizeof(path) / sizeof(wchar_t), L"/Logs/");
		if (_waccess_s(path, 0) != 0)
		{
			CreateDirectory(path, nullptr);
		}

		std::wstring file = path;
		file += date + L"_" + defaultFileName;
		os.open(file, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
	}

	if (!os.is_open()) return;

	os << GetTime(timePoint) << LogLevelToString(_logLevel) << _message << std::endl;
	os.flush();
}

std::wstring Logger::LogLevelToString(ClonixLogLevel _logLevel)
{
	switch (_logLevel)
	{
	case ClonixLogLevel::TRACE: return L" [TRACE] ";
	case ClonixLogLevel::DBG: return L" [DBG] ";
	case ClonixLogLevel::INFO: return L" [INFO] ";
	case ClonixLogLevel::WARN: return L" [WARN] ";
	case ClonixLogLevel::ERR: return L" [ERR] ";
	case ClonixLogLevel::FATAL: return L" [FATAL] ";
	}

	return L"";
}



std::wstring Logger::GetDate(const std::chrono::time_point<std::chrono::system_clock> _timePoint)
{
	const std::time_t now = std::chrono::system_clock::to_time_t(_timePoint);

	wchar_t buffer[16] = { 0, };
	std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y%m%d", std::localtime(&now));

	return buffer;
}

std::wstring Logger::GetTime(const std::chrono::time_point<std::chrono::system_clock> _timePoint)
{
	const std::time_t now = std::chrono::system_clock::to_time_t(_timePoint);
	auto millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(_timePoint.time_since_epoch()) % 1000;

	wchar_t buffer[16] = { 0, };
	std::wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%H:%M:%S", std::localtime(&now));

	wchar_t millisecBuffer[4] = { 0, };
	std::swprintf(millisecBuffer, sizeof(millisecBuffer) / sizeof(wchar_t), L"%03d", static_cast<int>(millisecond.count()));

	std::wstring result = L"[";
	result += buffer;
	result += L":";
	result += millisecBuffer;
	result += L"]";

	return result;
}

std::wstring Logger::stringToWString(const std::string& str) {
	std::wstring wstr(str.length(), L'\0');
	std::mbstowcs(&wstr[0], str.c_str(), str.size());
	return wstr;
}

std::wstring Logger::charToWString(const char _ch) {
	unsigned int value = _ch;
	std::wstringstream wss;

	wss << L"0x" << std::hex << std::setw(4) << std::setfill(L'0') << value;
	std::wstring hexString = wss.str();

	return hexString;
}

std::string Logger::charToString(const char _ch) {
	unsigned int value = _ch;
	std::stringstream ss;

	ss << "0x" << std::hex << std::setw(4) << std::setfill('0') << value;
	std::string hexString = ss.str();

	return hexString;
}
