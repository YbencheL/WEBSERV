#include "../client.hpp"
#include "includes/parseRequest.hpp"
#include "includes/request.hpp"


void replace_all(std::string &s, const std::string &from, const std::string &to)
{
    size_t pos = 0;

    if (from.empty())
        return;

    while ((pos = s.find(from, pos)) != std::string::npos)
    {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
}

void printClientInformation(Client &client)
{
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Method :" << client.req.getMethod() << std::endl;
    std::cout << "Path :" << client.req.getPath() << std::endl;
    std::cout << "Query :" << client.req.getQuery() << std::endl;
    std::cout << "HTTP version :" << client.req.getHttpVersion() << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    std::map<std::string, std::string>::const_iterator it;

    std::map<std::string, std::string> headers = client.req.getHeaders();

    std::cout << "----------- HEADERS -----------" << std::endl;

    for (it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "-------------------------------" << std::endl;
    std::cout << "body :" << client.req.getBody() << std::endl;
}

int main()
{
    Client      test;
    std::string testData;
    inisializeClient(test);

    // while (1)
    // {
    //     if (std::cin.eof())
    //         break;
    //     getline(std::cin, testData);
    //     replace_all(testData, "newline", "\r\n");
    //     int exitCode = parseRequest(test, testData);
    //     std::cout << "Exit code  :" << exitCode << std::endl;
    // }
    std::string s(
        "POST /abenzaho/..///index/./hi.html?var=hello "
        "HTTP/1.0\r\nhost:abenzaho\r\nTRANSFER-ENCODING: "
        "chunked\r\nuser:abenzaho\r\n\r\n5\r\nhello\r\n0\r\n\r\n",
        132
    );
    int exitCode = parseRequest(test, s);
    std::cout << "Exit code  :" << exitCode << std::endl;
    printClientInformation(test);
}

// POST /?var=hello
// H(TTP/1.0newlinehost:abenzahonewlineuser:abenzahonewlinenewline