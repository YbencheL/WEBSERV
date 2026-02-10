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

int main()
{
    client      test;
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
}