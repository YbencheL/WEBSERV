#ifndef CGI_HPP
#define CGI_HPP

#include "../client.hpp"

class Cgi
{
  private:
    std::string interpreter;
    std::string extension;
    char      **envp;
    char      **argv;

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
};

#endif