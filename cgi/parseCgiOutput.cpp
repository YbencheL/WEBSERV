#include "../client.hpp"
#include "../utils/utils.hpp"
#include "cgi.hpp"

bool isUnique(std::string &key, std::map<std::string, std::string> &header)
{
    return header.find(key) == header.end();
}

int Cgi::parseOutToken(std::string &token)
{
    if (token.size() > MAX_SINGLE_HEADER_SIZE)
        return INTERNAL_SERVER_ERROR;

    size_t col = token.find(":");
    if (col == std::string::npos)
        return INTERNAL_SERVER_ERROR;

    std::string name = token.substr(0, col);
    if (!checkNameField(name))
        return INTERNAL_SERVER_ERROR;

    UpperCaseHeaderName(name);
    if (name != "SET_COOKIE" && !isUnique(name, cgiHeaders))
        return INTERNAL_SERVER_ERROR;

    std::string value = token.substr(col + 1);
    trimLeft(value, "\t ");
    if (!checkValueField(value))
        return INTERNAL_SERVER_ERROR;

    if (name == "SET_COOKIE" && !cgiHeaders[name].empty())
    {
        cgiHeaders[name].append("; ");
        cgiHeaders[name].append(value);
    }
    else
        cgiHeaders[name] = value;

    if (name == "CONTENT_TYPE")
        contentType = true;
    if (name == "STATUS")
        OutStatus = true;
    return 0;
}

int Cgi::parseOutHeaders(std::string &headers)
{
    while (true)
    {
        size_t newL = headers.find("\n");
        size_t crLf = headers.find("\r\n");
        size_t breaker;
        size_t breakerSize;

        if (newL < crLf)
        {
            breaker     = newL;
            breakerSize = 1;
        }
        else
        {
            breaker     = crLf;
            breakerSize = 2;
        }
        std::string token = headers.substr(0, breaker);
        if (parseOutToken(token))
            return INTERNAL_SERVER_ERROR;
        headers.erase(0, breaker + breakerSize);
        if (headers.empty())
            return 0;
    }
}

void Cgi::addInfo()
{
    if (!contentType)
        cgiHeaders["CONTENT_TYPE"] = "text/plain";
    if (!OutStatus)
        cgiHeaders["STATUS"] = "200 OK";
}

int Cgi::parseOutput(std::string &output)
{
    size_t newL = output.find("\n\n");
    size_t crLf = output.find("\r\n\r\n");
    if (newL == std::string::npos && crLf == std::string::npos)
        return INTERNAL_SERVER_ERROR;

    size_t breaker;
    size_t breakerSize;

    if (newL < crLf)
    {
        breaker     = newL;
        breakerSize = 2;
    }
    else
    {
        breaker     = crLf;
        breakerSize = 4;
    }

    if (breaker > MAX_HEADER_SIZE)
        return INTERNAL_SERVER_ERROR;

    std::string headers = output.substr(0, breaker + (breakerSize / 2));
    if (parseOutHeaders(headers))
        return INTERNAL_SERVER_ERROR;

    addInfo();
    output.erase(0, breaker + breakerSize);
    
    return OUTPUT_READY;
}