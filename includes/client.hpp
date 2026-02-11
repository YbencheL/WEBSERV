#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "request.hpp"
#include "parseRequest.hpp"

class Request;
struct reqParse;

struct Client
{
    Request req;
	reqParse parse;
    // Response res;
    bool    req_ready;
};

void inisializeClient(Client &client);


#endif