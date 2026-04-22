#include "client.hpp"

void inisializeClient(Client &client)
{
    client.parse.bodyRead   = 0;
    client.parse.body       = false;
    client.reqReady         = false;
    client.parse.bodyBegin  = false;
    client.parse.step       = REQLINE;
    client.parse.chunkState = CALCULATING;
    client.cgiHandler.state = CHECKING;
}
