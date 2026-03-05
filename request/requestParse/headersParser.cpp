#include "../../client.hpp
#include "../../utils/utils.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"
#include <boost/algorithm/string.hpp>

void UpperCaseHeaderName(std::string &name)
{
    for (size_t i = 0; i < name.size(); i++)
    {
        if (name[i] <= 'z' && name[i] >= 'a')
            name[i] = toupper(name[i]);
    }
}

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

bool parseToken(
    std::string token, std::map<std::string, std::string> &headers, bool &host
)
{
    if (token.size() > MAX_SINGLE_HEADER_SIZE)
        return false;
    size_t pos = token.find(":");
    if (pos == std::string::npos)
        return false;
    std::string name = token.substr(0, pos);
    if (!checkNameField(name))
        return false;
    UpperCaseHeaderName(name);
    if (!checkForDouble(name, headers))
        return false;
    std::string value = token.substr(pos + 1);
    trimLeft(value, "\t ");
    if (!checkValueField(value))
        return false;
    if (name == "HOST")
        host = true;
    headers[name] = value;
    return true;
}

bool checkSetHeaders(int hn, Client &client, std::map<int, std::string> &tokens)
{
    bool                               host = false;
    std::map<std::string, std::string> headers;
    for (int i = 0; i < hn; i++)
    {
        if (!parseToken(tokens[i], headers, host))
            return false;
    }
    if (!host)
        return false;
    client.req.setHeader(headers);
    return true;
}

int checkMethodAllowed(Client &client)
{
    for(int i = 0;i < 3;i++)
    {
        if (client.req.getMethod() == client.location_conf->allow_methods[i])

    }
    return 1;
}

int parseHeaders(Client &client, std::string &data)
{
    std::map<int, std::string> tokens;
    int                        NumberOfTokens = splitDataToTokens(data, tokens);

    if (!checkSetHeaders(NumberOfTokens, client, tokens))
        return BAD_REQUEST;
    if (!validate_headers(client))
        return NOT_FOUND;
    return 1;
}
