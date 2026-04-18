#include "../../client.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"

int parseRequest(Client &client, std::string &recivedData)
{
    client.parse.remaining.append(recivedData);
    if (client.parse.step == REQLINE)
    {
        size_t newLinePos = client.parse.remaining.find("\r\n");
        if (newLinePos != std::string::npos)
        {
            std::string reqLine = client.parse.remaining.substr(0, newLinePos);
            if (reqLine.size() > MAX_REQ_SIZE)
                return URI_TOO_LARGE;
            int ERROR = parseRequestLine(client, reqLine);
            if (ERROR)
                return ERROR;
            client.parse.step = HEADERS;
            client.parse.remaining.erase(0, newLinePos);
        }
        else if (client.parse.remaining.size() > MAX_REQ_SIZE)
            return URI_TOO_LARGE;
        else
            return REQ_NOT_READY;
    }
    if (client.parse.step == HEADERS)
    {
        size_t headerEnd = client.parse.remaining.find("\r\n\r\n");
        if (headerEnd == 0)
            return BAD_REQUEST;
        else if (headerEnd != std::string::npos)
        {
            std::string headers =
                client.parse.remaining.substr(2, headerEnd + 2);

            if (headers.size() > MAX_HEADER_SIZE)
                return HEADER_TOO_LARGE;

            int EXIT_CODE = parseHeaders(client, headers);
            if (EXIT_CODE != 1)
                return EXIT_CODE;
            client.parse.step = BODY;
            client.parse.remaining.erase(0, headerEnd + 4);
        }
        else if (client.parse.remaining.size() > MAX_HEADER_SIZE)
            return HEADER_TOO_LARGE;
        else
            return REQ_NOT_READY;
    }
    if (client.parse.step == BODY && client.parse.body)
    {
        return parseBody(client);
    }
    return OK;
}
