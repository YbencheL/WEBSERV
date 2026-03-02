#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#define MAX_REQ_SIZE 5000
#define MAX_HEADER_SIZE 32000

#include <iostream>
#include <list>
#include <map>

enum parseSteps
{
    REQLINE,
    HEADERS,
    BODY
};

enum bodyChunkState
{
    CALCULATING,
    READING,
    FINALCRLF
};

struct Client;

struct reqParse
{
    parseSteps      step;
    bodyChunkState chunkState;
    size_t          expectedBytes;
    std::string     remaining;
    bool            body;
    std::string     methods[3];
    bool            bodyBegin;
    std::string     bodyReadMod;
    int             contentLength;
};

void UpperCaseHeaderName(std::string &name);
int  parseBody(Client &client);
bool parseHeaders(Client &client, std::string &data);
int  parseRequestLine(Client &client, std::string &data);
int  parseRequest(Client &client, std::string &recivedData);

#endif