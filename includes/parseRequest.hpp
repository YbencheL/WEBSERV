#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#include <iostream>

struct Client;

struct token
{
    std::string str;
    token      *next;
};

struct reqParse
{
    std::string       remaining;
    bool              reqLine;
    bool              body;
    std::string methods[3];
};

int    parseRequest(Client &client, std::string recivedData);
int    parseRequestLine(Client &client, std::string &data);
token *splitDataToTokens(std::string data);
int    rangeToken(int begin, int &end, std::string &data);
void   freeTokens(token *tokens);
token *newToken(std::string data);

#endif