// this main is not for testing or runing
//  only copy the code inside the main to put it in hte cgi section in wevserve

#include "cgi.hpp"
#include "../client.hpp"

int main()
{
    Client client;

    if (client.cgiHandler.state == CHECKING)
    {
        if (client.cgiHandler.checkForCgi(client))
            client.cgiHandler.state = SETUP_CGI;
    }
    if (client.cgiHandler.state == SETUP_CGI)
    {
        client.cgiHandler.buildEnv(client);
        client.cgiHandler.buildArg(client);
        client.cgiHandler.state = CREAT_PIPES;
    }
    if (client.cgiHandler.state == CREAT_PIPES)
    {
        if (!client.cgiHandler.creatPipes())
            client.cgiHandler.state = ERROR;
        else
            client.cgiHandler.state = EXECUTING;
    }
    if (client.cgiHandler.state == EXECUTING)
    {
        client.cgiHandler.pid = fork();
        if (client.cgiHandler.pid == -1)
        {
            std::cerr << "FORK FAILED" << std::endl;
            client.cgiHandler.state = ERROR;
        }
        if (client.cgiHandler.pid == 0)
            client.cgiHandler.childProccess();
        else
        {
            client.cgiHandler.parantProccess(client);
            client.cgiHandler.state = CGI_READING;
        }
    }
    if (client.cgiHandler.state == CGI_READING || client.cgiHandler.state == CGI_WAITING)
        client.cgiHandler.reading();
}
