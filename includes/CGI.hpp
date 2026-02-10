#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>

class CGI
{
  private:
    std::string method;
    std::string path;
    std::string query;
    std::string body;
    std::string headers;

  public:
    CGI();
    CGI(std::string &method,
        std::string &path,
        std::string &query,
        std::string &body,
        std::string &headers);
    CGI(const CGI &other);
    CGI &operator=(const CGI &other);
};

#endif