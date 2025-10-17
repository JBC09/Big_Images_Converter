//
// Created by LCH on 24. 10. 16.
//

#ifndef CASINOSERVICE_EXCEPTION_H
#define CASINOSERVICE_EXCEPTION_H

#include <exception>
#include <utility>

enum class InitializeErrorCode
{
    ControlBoard =100001,
    ConveyerRelay,
    Shutter,
    LedControlBoard,
    Keypad,
    Barcode,
    PassportReader,
    CardPrinter,
    FaceCamera,
    CardReader,
    TicketPrinter,
    TITOPrinter,
    TITOReader,
    LedStrip,
    NfcReader,
    ChipAcceptor
};


class CasinoServiceException : public std::exception
{
public:
    explicit CasinoServiceException(int errCode) : errCode(errCode) {}

    CasinoServiceException(int errCode, std::string description)
        : errCode(errCode),
          description(std::move(description))
    {
        fullMsg = std::to_string(errCode) + " " + this->description;
    }

    [[nodiscard]] const char* what() const override
    {
        return fullMsg.c_str();
    }

private:
    int errCode;
    std::string description;
    std::string fullMsg;

public:
    [[nodiscard]] int GetErrCode() const
    {
        return errCode;
    }

    [[nodiscard]] std::string GetDescription() const
    {
        return description;
    }
};

class InitializeException : public CasinoServiceException
{
public:
    explicit InitializeException(InitializeErrorCode errCode) : CasinoServiceException(static_cast<int>(errCode))
    {

    }
};

class InvalidArgumentException : public CasinoServiceException
{
public:
    explicit InvalidArgumentException(int errCode)
        : CasinoServiceException(errCode)
    {
    }
};

#endif //CASINOSERVICE_EXCEPTION_H
