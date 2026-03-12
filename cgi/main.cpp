// this main is not for testing or runing
//  only copy the code inside the main to put it in hte cgi section in wevserve

#include "cgi.hpp"

int main()
{
    Client client;
    Cgi    cgiHandler;
    if (client.state == CHECKING)
    {
        if (cgiHandler.checkForCgi(client))
            client.state = SETUP_CGI;
    }
    if (client.state == SETUP_CGI)
    {
        cgiHandler.buildEnv(client);
        cgiHandler.buildArg(client);
        client.state = CREAT_PIPES;
    }
    if (client.state == CREAT_PIPES)
    {
        if (!cgiHandler.creatPipes())
            client.state = ERROR;
        else
            client.state = EXECUTING;
    }
    if (client.state = EXECUTING)
    {
        int pid = fork();
        if (pid == -1)
        {
            std::cerr << "FORK ERROR" << std::endl;
            client.state = ERROR;
        }
        if (pid == 0)
            cgiHandler.childProccess();
        else
        {
            cgiHandler.parantProccess(client);
            client.state = CGI_READING;
        }
    }
    if (client.state = CGI_READING)
    {
        }
}
