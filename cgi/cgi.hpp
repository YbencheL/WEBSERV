#ifndef CGI_HPP
#define CGI_HPP

#define CGI_TIMEOUT      5
#define WRITE_READ_LIMIT 65000

#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <wait.h>

enum cgiState
{
    CHECKING,
    SETUP_CGI,
    CREAT_PIPES,
    EXECUTING,
    CGI_READY,
    CGI_WAITING,
    CGI_DONE,
    CGI_NOT_REQUIRED,
    ERROR
};

struct Client;

class Cgi
{
  private:
    std::string interpreter;
    std::string extension;
    std::string scriptPath;
    char      **envp;
    char      **argv;
    int         pipeIn[2];
    int         pipeOut[2];
    std::string response;

  public:
    static size_t  CGI_MAX_OUTPUT;
    cgiState       state;
    pid_t          pid;
    int            status;
    struct timeval start;
    struct timeval current;
    size_t         sent;

    Cgi();
    Cgi(const Cgi &other);
    Cgi &operator=(const Cgi &other);
    ~Cgi();

    void setInterpreter(const std::string &interpreter);
    void setExtension(const std::string &extension);

    std::string getInterpreter() const;
    std::string getExtension() const;
    char      **getArgv() const;
    char      **getEnv() const;

    void checkForCgi(Client &client);
    void buildEnv(Client &client);
    void buildArg();
    void setupCgi(Client &client);
    void createPipes();
    void execution(Client &client);
    void childProcess();
    void parentProcess(Client &client);
    void writing(Client &client);
    void checkResponseAndTime();
    void reading();
    void handleCGI(Client &client);

    int getPipeFd() const;
    int getPipeInFd() const;
};

#endif