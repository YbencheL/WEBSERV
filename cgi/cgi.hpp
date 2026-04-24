#ifndef CGI_HPP
#define CGI_HPP

#define CGI_TIMEOUT           5
#define WRITE_READ_LIMIT      65000
#define OUTPUT_NOT_READY      0
#define OUTPUT_READY          200
#define REDIRECTION           300
#define CLIENT_ERROR          400
#define INTERNAL_SERVER_ERROR 500

#include <iostream>
#include <map>
#include <stdlib.h>
#include <sys/epoll.h>
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
    bool           writeEnd;
    bool           safeExit;
    bool           closedAll;
    bool           sigTermSent;

    std::map<std::string, std::string> cgiHeaders;
    bool                               contentType;
    bool                               OutStatus;

    Cgi();
    Cgi(const Cgi &other);
    Cgi &operator=(const Cgi &other);
    ~Cgi();

    void setInterpreter(const std::string &interpreter);
    void setExtension(const std::string &extension);

    std::string  getInterpreter() const;
    std::string  getExtension() const;
    std::string &getCgiResponse();
    char       **getArgv() const;
    char       **getEnv() const;

    void checkForCgi(Client &client);
    void buildEnv(Client &client);
    void buildArg();
    void setupCgi(Client &client);
    void createPipes();
    void execution(Client &client);
    void childProcess();
    void parentProcess();
    void writing(int epoll_fd, unsigned int events, Client &client);
    void reading(unsigned int events, Client &clienty);
    void checkResponseAndTime(Client &client);
    void handleCGI(Client &client);
    int  getPipeOutFd() const;
    int  getPipeInFd() const;

    int  parseOutToken(std::string &token);
    int  parseOutHeaders(std::string &headers);
    void addInfo();
    int  parseOutput(std::string &output);
};

#endif