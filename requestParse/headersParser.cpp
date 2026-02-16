#include "../includes/client.hpp"
#include "../includes/parseRequest.hpp"
#include "../includes/request.hpp"

int splitDataToTokens(std::string &data, std::map<int, std::string> &tokens)
{
    size_t begin = 0;
    size_t end   = 0;
    int    i     = 0;
    for (; true; i++)
    {
        end = data.find("\r\n", begin);
        if (end == std::string::npos ||
            (begin - 2) == data.find("\r\n\r\n", begin - 2))
            break;
        tokens[i] = data.substr(begin, end - begin);
        begin     = end + 2;
    }
    return i;
}

bool checkNameField(std::string &name)
{
    static const std::string tspecials = "()<>@,;:\\\"/[]?={} \t";

    if (name.empty())
        return false;
    for (size_t i = 0; i < name.size(); i++)
    {
        if (!isprint(name[i]) || tspecials.find(name[i]) != std::string::npos)
            return false;
    }
    return true;
}

bool checkValueField(std::string &value)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!isprint(value[i]) && value[i] != 9)
            return false;
    }
    return true;
}

bool parseToken(std::string token, std::map<std::string, std::string> &headers)
{
    size_t pos = token.find(":");
    if (pos == std::string::npos)
        return false;
    std::string name = token.substr(0, pos);
    if (!checkNameField(name))
        return false;
    std::string value = token.substr(pos + 1);
    std::cout << value << std::endl;
    if (!checkValueField(value))
        return false;
    headers[name] = value;
    return true;
}

bool checkSetHeaders(int hn, Client &client, std::map<int, std::string> &tokens)
{
    std::map<std::string, std::string> headers;
    for (int i = 0; i < hn; i++)
    {
        if (!parseToken(tokens[i], headers))
            return false;
    }
    client.req.setHeader(headers);
    return true;
}

bool parseHeaders(Client &client, std::string &data)
{
    std::map<int, std::string> tokens;
    int                        NumberOfTokens = splitDataToTokens(data, tokens);

    //
    std::cout << NumberOfTokens << "---------------test-----------"
              << std::endl;
    for (int i = 0; i < NumberOfTokens; i++)
    {
        std::cout << tokens[i] << std::endl;
    }
    std::cout << "---------------test-----------" << std::endl;
    //

    if (!checkSetHeaders(NumberOfTokens, client, tokens))
        return false;
    return true;
}

// GET / HTTP/1.0newlinehost: abenzahonewlineuser:abenzahonewlinenewline