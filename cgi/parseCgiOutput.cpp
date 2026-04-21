#include "../client.hpp"
#include "../utils/utils.hpp"
#include "cgi.hpp"

int splitDataToTokens(std::string &data, std::map<int, std::string> &tokens)
{
    size_t begin = 0;
    size_t end   = 0;
    int    i     = 0;
    for (; true; i++)
    {
        end = data.find("\r\n", begin);
        if (end == std::string::npos ||
            (begin - 2) == data.find("\r\n\r\n", begin - 2))
            break;
        tokens[i] = data.substr(begin, end - begin);
        begin     = end + 2;
    }
    return i;
}

bool checkValueField(std::string &value)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!isprint(value[i]) && value[i] != 9)
            return false;
    }
    return true;
}

bool checkForDouble(
    std::string &key, std::map<std::string, std::string> &header
)
{
    return header.find(key) == header.end();
}

int parseToken(
    std::string token, std::map<std::string, std::string> &headers, bool &host
)
{
    if (token.size() > MAX_SINGLE_HEADER_SIZE)
        return HEADER_TOO_LARGE;
    size_t pos = token.find(":");
    if (pos == std::string::npos)
        return BAD_REQUEST;
    std::string name = token.substr(0, pos);
    if (!checkNameField(name))
        return BAD_REQUEST;
    UpperCaseHeaderName(name);
    if (name != "SET_COOKIE" && !checkForDouble(name, headers))
        return BAD_REQUEST;
    std::string value = token.substr(pos + 1);
    trimLeft(value, "\t ");
    if (!checkValueField(value))
        return BAD_REQUEST;
    if (name == "HOST")
        host = true;
    if (name == "COOKIE")
    {
        if (headers[name].empty())
            headers[name] = value;
        else
        {
            headers[name].append("; ");
            headers[name].append(value);
        }
    }
    else
        headers[name] = value;
    return 0;
}

int checkSetHeaders(int hn, Client &client, std::map<int, std::string> &tokens)
{
    bool                               host = false;
    int                                ERROR;
    std::map<std::string, std::string> headers;
    for (int i = 0; i < hn; i++)
    {
        ERROR = parseToken(tokens[i], headers, host);
        if (ERROR)
            return ERROR;
    }
    if (!host)
        return BAD_REQUEST;
    client.req.setHeader(headers);
    return 0;
}

//////////////////////////////////////////////

void trimLeft(std::string &str, std::string unwanted)
{
    size_t i = 0;
    for (; i < str.size(); i++)
    {
        if (unwanted.find(str[i]) == std::string::npos)
            break;
    }
    if (i == 0)
        return;
    str = str.substr(i);
}

void UpperCaseHeaderName(std::string &name)
{
    for (size_t i = 0; i < name.size(); i++)
    {
        if (name[i] <= 'z' && name[i] >= 'a')
            name[i] = toupper(name[i]);
        if (name[i] == '-')
            name[i] = '_';
    }
}

bool checkNameField(std::string &name)
{
    static const std::string tspecials = "()<>@,;:\\\"/[]?={} \t";

    if (name.empty())
        return false;
    for (size_t i = 0; i < name.size(); i++)
    {
        if (!isprint(name[i]) || tspecials.find(name[i]) != std::string::npos)
            return false;
    }
    return true;
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
    if (name != "SET_COOKIE" && !checkForDouble(name, cgiHeaders))
        return BAD_REQUEST;

    std::string value = token.substr(col + 1);
    trimLeft(value, "\t ");
    if (!checkValueField(value))
        return BAD_REQUEST;
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
            breakerSize = 2;
        }
        else
        {
            breaker     = crLf;
            breakerSize = 1;
        }
        std::string token = headers.substr(breaker);
        parseOutToken(token);
    }
}

int Cgi::parseOutput(std::string &output)
{
    size_t newL = output.find("\n\n");
    size_t crLf = output.find("\r\n\r\n");
    if (newL != std::string::npos || crLf != std::string::npos)
    {
        size_t breaker;
        size_t breakerSize;

        if (newL < crLf)
        {
            breaker     = newL;
            breakerSize = 4;
        }
        else
        {
            breaker     = crLf;
            breakerSize = 2;
        }
        if (breaker > MAX_HEADER_SIZE)
            return INTERNAL_SERVER_ERROR;
        std::string headers = output.substr(0, breaker + (breakerSize / 2));
        parseOutHeaders(headers);
    }
    else if (output.size() > MAX_HEADER_SIZE)
        return INTERNAL_SERVER_ERROR; // return error
    else
        return OUTPUT_NOT_READY;
}