#include "../../client.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"

int parseRequest(Client &client, std::string &recivedData)
{
    client.parse.remaining.append(recivedData);
    if (client.parse.step == REQLINE)
    {

        if (client.parse.remaining.size() > MAX_REQ_SIZE)
            return URI_TOO_LARGE;
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
            return REQ_NOT_READY;
    }
    if (client.parse.step == HEADERS)
    {
        if (client.parse.remaining.size() > MAX_HEADER_SIZE)
            return HEADER_TOO_LARGE;
        size_t headerEnd = client.parse.remaining.find("\r\n\r\n");
        if (headerEnd == 0)
        {
            client.reqReady = true;
            return OK;
        }
        else if (headerEnd != std::string::npos)
        {
            std::string headers =
                client.parse.remaining.substr(2, headerEnd + 2);
            int EXIT_CODE = parseHeaders(client, headers);
            if (EXIT_CODE != 1)
                return EXIT_CODE;
            client.parse.step = BODY;
            client.parse.remaining.erase(0, headerEnd + 4);
        }
        else
            return REQ_NOT_READY;
    }
    if (client.parse.step == BODY && client.parse.body)
    {
        if (client.parse.remaining.size() > (size_t)client.location_conf->client_max_body_size)
            return PAYLOAD_TOO_LARGE;
        return parseBody(client);
    }
    return OK;
}
