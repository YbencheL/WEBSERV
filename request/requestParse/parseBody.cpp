#include "../../client.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"

void UpperCaseBodyBytes(std::string &bytes)
{
    for (size_t i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] <= 'z' && bytes[i] >= 'a')
            bytes[i] = toupper(bytes[i]);
    }
}

int strToBase(std::string &str, int maxBody, std::string base)
{
    long   n     = 0;
    size_t i     = 0;
    int    nBase = base.size();
    int    match = 0;

    if (str.empty())
        return -1;
    for (; i < str.size(); i++)
    {
        for (match = 0; match < nBase; match++)
        {
            if (str[i] == base[match])
                break;
        }
        if (match == nBase)
            return -1;
        else
        {
            n = (n * nBase) + match;
            if (n > maxBody)
                return -1;
        }
    }
    return n;
}

bool checkForMethod(Client &client)
{
    std::map<std::string, std::string> headers = client.req.getHeaders();
    std::map<std::string, std::string>::iterator it;
    bool                                         flag = false;

    for (it = headers.begin(); it != headers.end(); it++)
    {
        if (it->first == "CONTENT-LENGTH")
        {
            if (flag)
                return false;
            int length = strToBase(it->second, 10000000, "0123456789");
            // TODO : add the config file max body later
            if (length == -1)
                return false;
            else
                client.parse.contentLength = length;
            client.parse.bodyReadMod = it->first;
            flag                     = true;
        }
        if (it->first == "TRANSFER-ENCODING" && it->second == "chunked")
        {
            if (flag)
                return false;
            client.parse.bodyReadMod = it->first;
            flag                     = true;
        }
    }
    if (flag)
        return true;
    return false;
}

int collectBodyByLength(Client &client)
{
    if (client.parse.remaining.size() >= (size_t)client.parse.contentLength)
    {
        client.req.setBody(
            client.parse.remaining.substr(0, client.parse.contentLength)
        );
        return 200;
    }
    else
        return 0;
}

int collectBodyByChunks(Client &client, std::string &remain)
{
    while (true)
    {
        if (client.parse.chunkState == CALCULATING)
        {
            size_t end = remain.find("\r\n");
            if (end == std::string::npos)
                return 0;
            std::string bytesLine = remain.substr(0, end);
            UpperCaseBodyBytes(bytesLine);
            int bytes = strToBase(bytesLine, 10000000, "0123456789ABCDEF");
            if (bytes == -1)
                return 400;
            remain.erase(0, end + 2);
            if (bytes == 0)
                client.parse.chunkState = FINALCRLF;
            else
                client.parse.chunkState = READING;
            client.parse.expectedBytes = bytes;
        }
        if (client.parse.chunkState == READING)
        {
            int bytes = client.parse.expectedBytes;
            if ((size_t)(bytes + 2) > remain.size())
                return 0;
            else
            {
                if (remain.compare(bytes, 2, "\r\n") != 0)
                    return 400;
                else
                {
                    client.req.appendBody(remain.substr(0, bytes));
                    client.parse.chunkState = CALCULATING;
                    remain.erase(0, bytes + 2);
                }
            }
        }
        if (client.parse.chunkState == FINALCRLF)
        {
            if (remain.size() < 2)
                return 0;
            if (remain.compare(0, 2, "\r\n") == 0)
                return 200;
            else
                return 400;
        }
    }
    return 0;
}

int parseBody(Client &client)
{
    if (!client.parse.bodyBegin)
    {
        if (!checkForMethod(client))
            return 400;
        else
            client.parse.bodyBegin = true;
    }

    if (client.parse.bodyBegin)
    {
        if (client.parse.bodyReadMod == "CONTENT-LENGTH")
            return collectBodyByLength(client);
        else if (client.parse.bodyReadMod == "TRANSFER-ENCODING")
            return collectBodyByChunks(client, client.parse.remaining);
    }
    return 0;
}