#include "../../client.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"

void normalisePath(std::string &path, std::string target, std::string rep, size_t size)
{
    size_t begin;
    while (true)
    {
        begin = path.find(target);
        if (begin != std::string::npos)
            path.replace(begin, size, rep);
        else
            break;
    }
}

int rangeToken(size_t begin, size_t &end, std::string &data)
{
    if (data[begin] == ' ')
        return 1;
    size_t i = begin + 1;
    for (; i < data.size(); i++)
    {
        if (data[i] == ' ')
            break;
    }
    if (i == begin)
        return 1;
    end = i;

    return 0;
}

bool splitDataToTokens(std::string data, std::map<int, std::string> &tokens)
{
    size_t end = 0;
    size_t begin = 0;

    for (int i = 0; i < 3; i++)
    {
        if (rangeToken(begin, end, data))
            return false;
        tokens[i] = data.substr(begin, end - begin);
        if ((end == data.size() || end + 1 == data.size()) && i < 2)
            return false;
        begin = end + 1;
    }
    if (end != data.size())
        return false;
    return true;
}

bool checkSetMethod(std::string &token, Client &client, std::string *methods)
{
    int i = 0;
    for (; i < 3; i++)
    {
        if (token == methods[i])
            break;
    }
    if (i == 3)
        return false;
    client.req.setMethod(token);
    if (token == "POST")
        client.parse.body = true;
    return true;
}

bool checkSetPathQuery(Client &cleint, std::string &data)
{
    // TODO : need to know if query should be flaged exist or not later
    if (data[0] != '/')
        return false;
    if (data.find('\r') != std::string::npos ||
        data.find('\n') != std::string::npos)
        return false;
    std::string path = data;
    if (data.find('?') != std::string::npos)
    {
        int queryStart = data.find('?');
        path = data.substr(0, queryStart);
        cleint.req.setQuery(data.substr(queryStart + 1));
    }
    normalisePath(path, "/./", "/", 3);
    normalisePath(path, "//", "/", 2);
    normalisePath(path, "%20", " ", 3);
    cleint.req.setPath(path);
    return true;
}

int checkSetHttp(Client &client, std::string token)
{
    std::string http[3] = {"HTTP/2.0", "HTTP/3.0"};
    if (token == "HTTP/1.0" || token == "HTTP/1.1")
    {
        client.req.setHttpVersion(token);
        return 0;
    }
    for (int i = 0; i < 3; i++)
    {
        if (token == http[i])
            return VERSION_NOT_SUPP;
    }
    return BAD_REQUEST;
}

int parseRequestLine(Client &client, std::string &data)
{
    std::map<int, std::string> tokens;
    if (!splitDataToTokens(data, tokens))
        return BAD_REQUEST;
    if (!checkSetMethod(tokens[0], client, client.parse.methods))
        return METHOD_NOT_ALLOWED;
    if (!checkSetPathQuery(client, tokens[1]))
        return BAD_REQUEST;
    int httpEC = checkSetHttp(client, tokens[2]);
    if (httpEC)
        return httpEC;
    return 0;
}
