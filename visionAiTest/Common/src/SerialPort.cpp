#include "SerialPort.h"
#include "Logger.h"

SerialPort::SerialPort()
    : handle(INVALID_HANDLE_VALUE),
      isInitializing(false)
{

}

SerialPort::~SerialPort()
{
    Release();
}


bool SerialPort::Init(const std::string& _name)
{
    if (isInitializing.load())
        return false;
    if (_name.empty()) return false;
    if (handle != INVALID_HANDLE_VALUE) return true;

    isInitializing = true;

    const int maxRetries = 6;
    int attempts = 0;

    while (attempts < maxRetries)
    {
        handle = CreateFileA((R"(\\.\)" + _name).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0,
                             nullptr);
        if (handle != INVALID_HANDLE_VALUE)
        {
            SetupComm(handle, 8192, 8192);
            PurgeComm(handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
            Logger::GetInstance().Info("[SerialPort::Init] [" + _name + "] Init Success");
            isInitializing.store(false);
            return true;
        }
        Logger::GetInstance().Error("[SerialPort::Init] [" + _name + "] CreateFile Error : " + std::to_string(GetLastError()));
        Logger::GetInstance().Error("[SerialPort::Init] [" + _name + "] Wait and Retry... " +
            std::to_string(attempts) + "/" + std::to_string(maxRetries));
        attempts++;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    Logger::GetInstance().Info("[SerialPort::Init] [" + _name + "] Init Fail");

    isInitializing.store(false);
    return false;
}

bool SerialPort::Release()
{
    if (handle == INVALID_HANDLE_VALUE) return true;

    if (!CloseHandle(handle)) {
        Logger::GetInstance().Error(L"[SerialPort::Release] CloseHandle Error : " + std::to_wstring(GetLastError()));
        return false;
    }
    handle = INVALID_HANDLE_VALUE;

    Logger::GetInstance().Info(L"[SerialPort::Release] Done");

    return true;
}



bool SerialPort::SetDcb(const DCB& _dcb)
{
    if (handle == INVALID_HANDLE_VALUE) {
        Logger::GetInstance().Error(L"[SerialPort::SetDcb] handle is null");
        return false;
    }

    DCB dcb;
    if (!GetCommState(handle, &dcb))
    {
        Logger::GetInstance().Error(L"[SerialPort::SetDcb] GetCommState Error : " + std::to_wstring(GetLastError()));
        return false;
    }

    dcb.BaudRate = _dcb.BaudRate;
    dcb.ByteSize = _dcb.ByteSize;
    dcb.Parity = _dcb.Parity;
    dcb.StopBits = _dcb.StopBits;
    dcb.fBinary = false;
    dcb.fOutxCtsFlow = false;
    dcb.fOutxDsrFlow = false;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fInX = false;
    dcb.fOutX = false;
    dcb.fNull = false;

    if (!SetCommState(handle, &dcb))
    {
        Logger::GetInstance().Error(L"[SerialPort::SetDcb] SetCommState Error : " + std::to_wstring(GetLastError()));
        return false;
    }

    return true;
}

bool SerialPort::SetTimeout(const COMMTIMEOUTS& _timeOuts)
{
    if (handle == INVALID_HANDLE_VALUE) {
        Logger::GetInstance().Error(L"[SerialPort::SetTimeout] handle is null");
        return false;
    }

    COMMTIMEOUTS timeOuts;
    if (!GetCommTimeouts(handle, &timeOuts))
    {
        Logger::GetInstance().Error(L"[SerialPort::SetTimeout] GetCommTimeouts Error : " + std::to_wstring(GetLastError()));
        return false;
    }

    timeOuts.ReadIntervalTimeout = _timeOuts.ReadIntervalTimeout;
    timeOuts.ReadTotalTimeoutMultiplier = _timeOuts.ReadTotalTimeoutMultiplier;
    timeOuts.ReadTotalTimeoutConstant = _timeOuts.ReadTotalTimeoutConstant;
    //timeOuts.WriteTotalTimeoutMultiplier = _timeOuts.WriteTotalTimeoutMultiplier;
    //timeOuts.WriteTotalTimeoutConstant = _timeOuts.WriteTotalTimeoutConstant;
    if (!SetCommTimeouts(handle, &timeOuts))
    {
        Logger::GetInstance().Error(L"[SerialPort::SetTimeout] SetCommTimeouts Error : " + std::to_wstring(GetLastError()));
        return false;
    }

    return true;
}


int SerialPort::Read(char* _buffer, int _len)
{
    if (handle == INVALID_HANDLE_VALUE) return 0;

    int readByte = 0;
    if (!ReadFile(handle, _buffer, _len, (DWORD*)&readByte, NULL))
    {
        return 0;
    }

    return readByte;
}

DWORD SerialPort::Read(char* _buffer)
{
    char buffer[256] = { 0x00, };
    DWORD bytes_read = 0L;

    if (handle == INVALID_HANDLE_VALUE) {
        Logger::GetInstance().Error(L"[SerialPort::Read] handle is null");
        return 0;
    }
    
    if (!ReadFile(handle, buffer, sizeof(buffer), &bytes_read, NULL))
    {
        Logger::GetInstance().Error(L"[SerialPort::Read] ReadFile Error : " + std::to_wstring(GetLastError()));
        return 0;
    }

    if (bytes_read > 0) {
        memcpy(_buffer, buffer, bytes_read);
    }
    else {
        Logger::GetInstance().Warning(L"[SerialPort::Read] ReadFile result 0");
    }

    //Logger::GetInstance().Info(L"[SerialPort::Read] result : " + std::to_wstring(bytes_read));

    return bytes_read;
}

int SerialPort::SlowRead(char* _buffer, int _len, int _timeOut)
{
    if (handle == INVALID_HANDLE_VALUE) {
        //Logger::GetInstance().Error(L"[SerialPort::SlowRead] handle is null");
        return 0;
    }

    int pos = 0;

    ULONGLONG start = GetTickCount64();
    while (true)
    {
        int readByte = Read(&_buffer[pos], _len);

        _len -= readByte;
        pos += readByte;

        if (_len <= 0) break;
        if (GetTickCount64() - start > _timeOut) break;
    }

    //Logger::GetInstance().Info(L"[SerialPort::SlowRead] pos : " + std::to_wstring(pos));

    return pos;
}

int  SerialPort::Write(char* _buffer, int _len)
{
    std::lock_guard lock(serialMutex);
    if (handle == INVALID_HANDLE_VALUE) {
        Logger::GetInstance().Error(L"[SerialPort::Write] handle is null");        
        return 0;
    }

    int writeByte = 0;
    if (!WriteFile(handle, _buffer, _len, (DWORD*)&writeByte, NULL))
    {
        Logger::GetInstance().Error(L"[SerialPort::Write] WriteFile Error : " + std::to_wstring(GetLastError()));

        FlushFileBuffers(handle); //conan 221213 add for serial comnunication speed-up...
        return 0;
    }

    //Logger::GetInstance().Info(L"[SerialPort::Write] result : " + std::to_wstring(writeByte));    

    return writeByte;
}

int SerialPort::Write(const unsigned char* _buffer, int _len)
{
    std::lock_guard lock(serialMutex);
    if (handle == INVALID_HANDLE_VALUE) {
        Logger::GetInstance().Error(L"[SerialPort::Write] handle is null");
        return 0;
    }

    int writeByte = 0;
    if (!WriteFile(handle, _buffer, _len, (DWORD*)&writeByte, NULL))
    {
        Logger::GetInstance().Error(L"[SerialPort::Write] WriteFile Error : " + std::to_wstring(GetLastError()));

        FlushFileBuffers(handle); //conan 221213 add for serial comnunication speed-up...
        return 0;
    }

    //Logger::GetInstance().Info(L"[SerialPort::Write] result : " + std::to_wstring(writeByte));

    return writeByte;
}

void SerialPort::Purge(const bool rx, const bool tx) const
{
    DWORD flags = 0;
    if (rx) flags |= PURGE_RXCLEAR;
    if (tx) flags |= PURGE_TXCLEAR;
    if (flags != 0 && handle != INVALID_HANDLE_VALUE)
        PurgeComm(handle, flags);
}
