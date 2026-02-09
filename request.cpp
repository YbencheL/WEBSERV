#include "request.hpp"

Request::Request()
{
}

Request::Request(const std::string &)
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

void Request::setHeader(std::string str)
{
    (void)str;
}

void Request::setHttpVersion(std::string str)
{
    httpVersion = str;
}

int pareRequest(client &client, std::string recivedData)
// return Error code when somethig wrong
{
    if (client.req_line == false)
    {
        client.remaining.append(recivedData);
        if (client.remaining.find("\r\n"))
        {
            client.req_line = true;
            //call request line parser
        }
    }
    
    return 200;
}