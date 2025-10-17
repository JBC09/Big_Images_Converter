#pragma once
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <string>

class SerialPort
{
public:
    SerialPort();
    ~SerialPort();

private:
    HANDLE handle;
    std::atomic<bool> isInitializing;
    std::mutex serialMutex;

public:
    bool Init(const std::string& _name);
    bool Release();

public:
    bool SetDcb(const DCB& _dcb);
    bool SetTimeout(const COMMTIMEOUTS& _timeOuts);

public:
    DWORD Read(char* _buffer);
    int Read(char* _buffer, int _len);
    int SlowRead(char* _buffer, int _len, int _timeOut);
    int Write(char* _buffer, int _len);
    int Write(const unsigned char* _buffer, int _len);
    void Purge(bool rx = true, bool tx = true) const;
};
