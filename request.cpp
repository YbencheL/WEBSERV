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

void Request::parseRawRequest()
{
    
}