#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

class Request
{
  private:
    std::string rawRequest;
    std::string method;
    std::string path;
    std::string query;
    std::string body;
    std::string headers;

  public:
    Request();
    Request(const std::string &rRequest);
    Request(const Request &);
    Request &operator=(const Request &);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getQuery() const;
    std::string getBody() const;
    std::string getHeaders() const;

    void setMethod(std::string str);
    void setPath(std::string str);
    void setQuery(std::string str);
    void setBody(std::string str);
    void setHeader(std::string str);

    void parseRawRequest();
};

#endif