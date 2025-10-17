//
// Created by 이채호 on 24. 10. 18.
//

#ifndef DYNAMIC_SERVER_IHTTPSERVER_H
#define DYNAMIC_SERVER_IHTTPSERVER_H

#include <string>
#include <functional>
#include <map>

enum class Status
{
    CONTINUE                      = 100,
    SWITCHING_PROTOCOLS           = 101,
    OK                            = 200,
    CREATED                       = 201,
    ACCEPTED                      = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT                    = 204,
    RESET_CONTENT                 = 205,
    PARTIAL_CONTENT               = 206,
    MULTIPLE_CHOICES              = 300,
    MOVED_PERMANENTLY             = 301,
    FOUND                         = 302,
    SEE_OTHER                     = 303,
    NOT_MODIFIED                  = 304,
    TEMPORARY_REDIRECT            = 307,
    PERMANENT_REDIRECT            = 308,
    BAD_REQUEST                   = 400,
    UNAUTHORIZED                  = 401,
    FORBIDDEN                     = 403,
    NOT_FOUND                     = 404,
    METHOD_NOT_ALLOWED            = 405,
    NOT_ACCEPTABLE                = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    CONFLICT                      = 409,
    GONE                          = 410,
    PAYLOAD_TOO_LARGE             = 413,
    UNSUPPORTED_MEDIA_TYPE        = 415,
    RANGE_NOT_SATISFIABLE         = 416,
    EXPECTATION_FAILED            = 417,
    PRECONDITION_REQUIRED         = 428,
    TOO_MANY_REQUESTS             = 429,
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    INTERNAL_SERVER_ERROR         = 500,
    NOT_IMPLEMENTED               = 501,
    BAD_GATEWAY                   = 502,
    SERVICE_UNAVAILABLE           = 503,
    GATEWAY_TIMEOUT               = 504,
    VARIANT_ALSO_NEGOTIATES       = 506
};

// 추상화된 Request 클래스
class IHttpRequest
{
public:
    [[nodiscard]] virtual std::string GetUrl() const = 0;
    [[nodiscard]] virtual std::map<std::string, std::string> GetParams() const = 0;
    [[nodiscard]] virtual std::string GetBody() const = 0;
    [[nodiscard]] virtual std::map<std::string, std::string> GetHeaders() const = 0;
    virtual ~IHttpRequest() = default;
};

// 추상화된 Response 클래스
class IHttpResponse
{
public:
    virtual void SetBody(const std::string& body) = 0;
    virtual void SetStatus(Status status) = 0;
    virtual void SetStatus(int status) = 0;
    virtual ~IHttpResponse() = default;
};

enum class HttpMethod
{
    Get,
    Post,
    Put,
    Delete,
    Patch
};

class IHttpServer
{
public:
    virtual void AddEndpoint(
        HttpMethod method,
        std::string url,
        std::function<void(const IHttpRequest&, IHttpResponse&)> handler
    ) = 0;

    virtual void Start(int port) = 0;
    virtual void Stop() = 0;

    virtual ~IHttpServer() = default;
};

#endif //DYNAMIC_SERVER_IHTTPSERVER_H
