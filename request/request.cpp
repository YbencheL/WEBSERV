#include "includes/request.hpp"

Request::Request()
{
}

Request::Request(const std::string &)
{
}

Request::Request(const Request &other)
{
    *this = other;
}

Request &Request::operator=(const Request &other)
{
    if (this != &other)
    {
        method      = other.method;
        path        = other.path;
        query       = other.query;
        httpVersion = other.httpVersion;
        body        = other.body;
    }
    return *this;
}

std::string Request::getMethod() const
{
    return method;
}

std::string Request::getPath() const
{
    return path;
}

std::string Request::getQuery() const
{
    return query;
}

std::string Request::getBody() const
{
    return body;
}

std::map<std::string, std::string> Request::getHeaders() const
{
    return headers;
}

std::string Request::getHttpVersion() const
{
    return httpVersion;
}

void Request::setMethod(std::string str)
{
    method = str;
}

void Request::setPath(std::string str)
{
    path = str;
}

void Request::setQuery(std::string str)
{
    query = str;
}

void Request::setBody(std::string str)
{
    body = str;
}

void Request::setHeader(std::map<std::string, std::string> map)
{
    headers = map;
}

void Request::setHttpVersion(std::string str)
{
    httpVersion = str;
}

void Request::appendBody(std::string str)
{
    body.append(str);
}