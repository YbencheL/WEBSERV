#include "request.hpp"
#include <iostream>

int main()
{
    std::string rawRequest = "DELETE /index.html?var=hello HTTP/1.1\r\n"
                             "Host: localhost\r\n"
                             "User-Agent: MyCppClient/1.0\r\n"
                             "\r\n"
                             "body\r\n"
                             "body\r\n";
    Request     request(rawRequest);
    request.parseRawRequest();
    std::cout << "method : " << request.getMethod() << "." << std::endl;
    std::cout << "path : " << request.getPath() << "." << std::endl;
    std::cout << "query : " << request.getQuery() << "." << std::endl;
    std::cout << "HTTP Version : " << request.getHttpVersion() << "."
              << std::endl;
    std::cout << "Headers : " << request.getHeaders() << "." << std::endl;
    std::cout << "Body : " << request.getBody() << "." << std::endl;
}