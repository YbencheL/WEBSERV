#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Request;
struct reqParse;

struct client
{
    Request req;
	reqParse parse;
    // Response res;
    bool    req_ready = false;
};

#endif