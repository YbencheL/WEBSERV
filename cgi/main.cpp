// this main is not for testing or runing
//  only copy the code inside the main to put it in hte cgi section in wevserve

#include "cgi.hpp"

void childProccess(Cgi cgiHnadler, int pipeIn[], int pipeOut[])
{
    dup2(STDIN_FILENO, pipeIn[0]);
    dup2(STDOUT_FILENO, pipeOut[1]);

    close(pipeIn[1]);
    close(pipeIn[0]);
    close(pipeOut[0]);
    close(pipeOut[1]);

    execve(cgiHnadler.getArgv()[0], cgiHnadler.getArgv(), cgiHnadler.getEnv());
    std::cerr << "execve failed: " << strerror(errno);
    exit(1);
}

int main()
{
    Client client;
    Cgi    cgiHandler;

    if (cgiHandler.checkForCgi(client))
    {
        cgiHandler.buildEnv(client);
        cgiHandler.buildArg(client);

        int pipeIn[2];
        int pipeOut[2];

        pipe(pipeIn);
        pipe(pipeOut);

        int pid = fork();
        if (pid == 0) // childe procces
        {
            childProccess(cgiHandler, pipeIn, pipeOut);
        }

        close(pipeIn[0]);
        close(pipeOut[1]);
        write(
            pipeIn[1], client.req.getBody().c_str(), client.req.getBody().size()
        );
        close(pipeIn[1]);
    }
}