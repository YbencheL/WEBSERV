#include "includes/client.hpp"
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

void    printClientInformation(Client client)
{
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Method :" << client.req.getMethod() << std::endl;
    std::cout << "Path :" << client.req.getPath() << std::endl;
    std::cout << "Query :" << client.req.getQuery() << std::endl;
    std::cout << "HTTP version :" << client.req.getHttpVersion() << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
}

int main()
{
    Client      test;
    std::string testData;
    inisializeClient(test);

    while (1)
    {
        if (std::cin.eof())
            break;
        getline(std::cin, testData);
        replace_all(testData, "newline", "\r\n");
        int exitCode = parseRequest(test, testData);
		std::cout << "Exit code  :" << exitCode << std::endl;
    }
    printClientInformation(test);
}