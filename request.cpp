#include "request.hpp"

Request::Request()
{
}

Request::Request(const std::string &rRequest) : rawRequest(rRequest)
{
}

Request::Request(const Request &)
{
}

Request &Request::operator=(const Request &)
{
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

std::string Request::getHeaders() const
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

void Request::setHeader(std::string str)
{
    headers = str;
}

void Request::setHttpVersion(std::string str)
{
    httpVersion = str;
}

void Request::parseRawRequest()
{
    //get Method
    setMethod(rawRequest.substr(0, rawRequest.find(' ')));
    rawRequest.erase(0, rawRequest.find(' ') + 1);
    //get path and query
    size_t end = rawRequest.find('?');
    if (end != rawRequest.npos && end < rawRequest.find(' '))
    {
        setPath(rawRequest.substr(0, end));
        setQuery(rawRequest.substr(end, rawRequest.find(' ') - end));
    }
    else
        setPath(rawRequest.substr(0, rawRequest.find(' ')));
    rawRequest.erase(0, rawRequest.find(' ') + 1);
    //get hhtp version
    setHttpVersion(rawRequest.substr(0, rawRequest.find("\r\n")));
    rawRequest.erase(0, rawRequest.find("\r\n") + 2);
    //get headers
    setHeader(rawRequest.substr(0, rawRequest.find("\r\n\r\n")));
    rawRequest.erase(0, rawRequest.find("\r\n\r\n") + 4);
    //get body
    std::cout << rawRequest << std::endl;
    setBody(rawRequest);
}