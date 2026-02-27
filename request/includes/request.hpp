#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>

class Request
{
  private:
    std::string                        method;
    std::string                        path;
    std::string                        query;
    std::string                        body;
    std::string                        httpVersion;
    std::map<std::string, std::string> headers;

  public:
    Request();
    Request(const std::string &);
    Request(const Request &other);
    Request &operator=(const Request &other);

    std::string                        getMethod() const;
    std::string                        getPath() const;
    std::string                        getQuery() const;
    std::string                        getBody() const;
    std::string                        getHttpVersion() const;
    std::map<std::string, std::string> getHeaders() const;

    void setMethod(std::string str);
    void setPath(std::string str);
    void setQuery(std::string str);
    void setBody(std::string str);
    void setHeader(std::map<std::string, std::string> map);
    void setHttpVersion(std::string str);

    void  appendBody(std::string str);
};

#endif