#include "../includes/client.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"

int parseRequest(Client &client, std::string recivedData)
{
    client.parse.remaining.append(recivedData);
    if (client.parse.step == REQLINE)
    {
        size_t newLinePos = client.parse.remaining.find("\r\n");
        if (newLinePos != std::string::npos)
        {
            std::string reqLine = client.parse.remaining.substr(0, newLinePos);
            int         ERROR   = parseRequestLine(client, reqLine);
            if (ERROR)
                return ERROR;
            client.parse.step = HEADERS;
            client.parse.remaining.erase(0, newLinePos);
        }
        else
            return 0;
    }
    if (client.parse.step == HEADERS)
    {
        std::cout << "remaining :" << client.parse.remaining << std::endl;// debug

        size_t headerEnd = client.parse.remaining.find("\r\n\r\n");
        if (headerEnd == 0)
        {
            client.reqReady = true;
            return 200;
        }
        else if (headerEnd != std::string::npos)
        {
            std::string headers = client.parse.remaining.substr(2, headerEnd + 2);
            parseHeaders(client, headers);
        }
        else
            return 0;
    }
    else if (client.parse.step == BODY)
    {
        return 0;
    }
    return 200;
}