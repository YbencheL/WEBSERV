#ifndef CGI_HPP
#define CGI_HPP

#include "../client.hpp"

class Cgi
{
  private:
    std::string interpreter;
    std::string extension;
    char **envp;

  public:
    Cgi();
    Cgi(const Cgi &other);
    Cgi &operator=(const Cgi &other);
    ~Cgi();

    void setInterpreter(const std::string &interpreter);
    void setExtension(const std::string &extension);

    std::string getInterpreter() const;
    std::string getExtension() const;

	bool checkForCgi(Client &client);
  void buildEnv(Client &client);
};

#endif