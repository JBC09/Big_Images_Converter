#pragma once
#include <fstream>
#include <chrono>
#include "ISingleton.h"
#include <chrono>
enum class ClonixLogLevel
{
	TRACE,
	DBG,
	INFO,
	WARN,
	ERR,
	FATAL
};

class Logger final :public ISingleton<Logger>
{
	friend class ISingleton<Logger>;

private:
	explicit Logger();

public:
	virtual ~Logger() override;

private:
	std::mutex mutex;
	std::wofstream os;

private:
	std::wstring defaultFileName;
	ClonixLogLevel logLevel;
	std::wstring date;

public:
	void SetDefaultFileName(const std::wstring& _defaultFileName);
	void SetLogLevel(const ClonixLogLevel& _logLevel);

public:
	void Trace(const std::string& _message);
	void Trace(const std::wstring& _message);
	void Debug(const std::string& _message);
	void Debug(const std::wstring& _message);
	void Info(const std::string& _message);
	void Info(const std::wstring& _message);
	void Warning(const std::string& _message);
	void Warning(const std::wstring& _message);
	void Error(const std::string& _message);
	void Error(const std::wstring& _message);
	void Fatal(const std::string& _message);
	void Fatal(const std::wstring& _message);
	void Close();

private:
	void Write(ClonixLogLevel _logLevel, const std::string& _message);
	void Write(ClonixLogLevel _logLevel, const std::wstring& _message);
	std::wstring LogLevelToString(ClonixLogLevel _logLevel);

private:
	std::wstring GetDate(const std::chrono::time_point<std::chrono::system_clock> _timePoint);
	std::wstring GetTime(const std::chrono::time_point<std::chrono::system_clock> _timePoint);

public:
	std::wstring stringToWString(const std::string& str);
	std::wstring charToWString(const char _ch);
	std::string charToString(const char _ch);
};

inline Logger& GetLogger()
{
	return Logger::GetInstance();
}
