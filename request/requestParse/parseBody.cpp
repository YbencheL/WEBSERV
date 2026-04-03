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
        return -2;
    for (; i < str.size(); i++)
    {
        for (match = 0; match < nBase; match++)
        {
            if (str[i] == base[match])
                break;
        }
        if (match == nBase)
            return -2;
        else
        {
            n = (n * nBase) + match;
            if (n > maxBody)
                return -1;
        }
    }
    return n;
}

int checkForMethod(Client &client)
{
    std::map<std::string, std::string> headers = client.req.getHeaders();
    std::map<std::string, std::string>::iterator it;
    bool                                         flag = false;

    for (it = headers.begin(); it != headers.end(); it++)
    {
        if (it->first == "CONTENT_LENGTH")
        {
            if (flag)
                return BAD_REQUEST;
            int length = strToBase(
                it->second,
                client.location_conf->client_max_body_size,
                "0123456789"
            );
            if (length == -1)
                return PAYLOAD_TOO_LARGE;
            else if (length == -2)
                return BAD_REQUEST;
            else
                client.parse.contentLength = length;
            client.parse.bodyReadMod = it->first;
            flag                     = true;
        }
        if (it->first == "TRANSFER_ENCODING" && it->second == "chunked")
        {
            if (flag)
                return BAD_REQUEST;
            client.parse.bodyReadMod = it->first;
            flag                     = true;
        }
    }
    if (flag)
        return 0;
    return BAD_REQUEST;
}

int collectBodyByLength(Client &client)
{
    if (client.parse.remaining.size() >= (size_t)client.parse.contentLength)
    {
        client.req.setBody(
            client.parse.remaining.substr(0, client.parse.contentLength)
        );
        return OK;
    }
    else
        return REQ_NOT_READY;
}

int collectBodyByChunks(Client &client, std::string &remain)
{
    while (true)
    {
        if (client.parse.chunkState == CALCULATING)
        {
            size_t end = remain.find("\r\n");
            if (end == std::string::npos)
                return REQ_NOT_READY;
            std::string bytesLine = remain.substr(0, end);
            if (bytesLine.size() > MAX_CHUNK_SIZE)
                return BAD_REQUEST;
            UpperCaseBodyBytes(bytesLine);
            int bytes = strToBase(
                bytesLine,
                client.location_conf->client_max_body_size,
                "0123456789ABCDEF"
            );
            if (bytes == -1)
                return BAD_REQUEST;
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
                return REQ_NOT_READY;
            else
            {
                if (remain.compare(bytes, 2, "\r\n") != 0)
                    return BAD_REQUEST;
                else
                {
                    client.req.appendBody(remain.substr(0, bytes));
                    if (client.req.getBody().size() >
                        (size_t)client.location_conf->client_max_body_size)
                        return PAYLOAD_TOO_LARGE;
                    client.parse.chunkState = CALCULATING;
                    remain.erase(0, bytes + 2);
                }
            }
        }
        if (client.parse.chunkState == FINALCRLF)
        {
            if (remain.size() < 2)
                return REQ_NOT_READY;
            if (remain.compare(0, 2, "\r\n") == 0)
                return OK;
            else
                return BAD_REQUEST;
        }
    }
    return 0;
}

int parseBody(Client &client)
{
    if (!client.parse.bodyBegin)
    {
        int ERROR = checkForMethod(client);
        if (ERROR)
            return ERROR;
        else
            client.parse.bodyBegin = true;
    }

    if (client.parse.bodyBegin)
    {
        if (client.parse.bodyReadMod == "CONTENT_LENGTH")
            return collectBodyByLength(client);
        else if (client.parse.bodyReadMod == "TRANSFER_ENCODING")
            return collectBodyByChunks(client, client.parse.remaining);
    }
    return 0;
}
