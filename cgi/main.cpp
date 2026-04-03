// this main is not for testing or runing
//  only copy the code inside the main to put it in hte cgi section in wevserve

#include "../client.hpp"
#include "cgi.hpp"

int main()
{
    Client client;

    if (client.cgiHandler.state == CHECKING)
        client.cgiHandler.checkForCgi(client);
    if (client.cgiHandler.state == SETUP_CGI)
        client.cgiHandler.setupCgi(client);
    if (client.cgiHandler.state == CREAT_PIPES)
        client.cgiHandler.createPipes();
    if (client.cgiHandler.state == EXECUTING)
        client.cgiHandler.execution(client);
}
