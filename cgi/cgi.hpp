#ifndef CGI_HPP
#define CGI_HPP

#include "../client.hpp"
#include <wait.h>

class Cgi
{
  private:
    std::string interpreter;
    std::string extension;
    char      **envp;
    char      **argv;
    int         pipeIn[2];
    int         pipeOut[2];

  public:
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

    bool checkForCgi(Client &client);
    void buildEnv(Client &client);
    void buildArg(Client &client);
    bool creatPipes();
    void childProccess();
    void parantProccess(Client &client);
};

#endif