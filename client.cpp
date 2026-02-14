#include "includes/client.hpp"

void inisializeClient(Client &client)
{
    client.parse.body       = false;
    client.reqReady        = false;
    client.parse.step = REQLINE;
    client.parse.methods[0] = "GET";
    client.parse.methods[1] = "POST";
    client.parse.methods[2] = "DELETE";
}

