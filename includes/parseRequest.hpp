#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#include <iostream>
#include <list>
#include <map>

enum parseSteps
{
    REQLINE,
    HEADERS,
    BODY
};

struct Client;

struct reqParse
{
    parseSteps  step;
    std::string remaining;
    bool        body;
    std::string methods[3];
};

bool parseHeaders(Client &client, std::string &data);
int  parseRequestLine(Client &client, std::string &data);
int  parseRequest(Client &client, std::string recivedData);

#endif