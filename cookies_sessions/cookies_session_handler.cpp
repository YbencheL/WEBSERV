#include "cookies_session_handler.hpp"

std::vector<std::string> split(std::vector<std::string> keywords, char dilimeter)
{
    size_t i = 0;
    std::string value;
    std::vector<std::string> splited_cont;


    while(i < keywords.size())
    {
        if (keywords[i] == "Cookie:")
            i++;
        std::istringstream stream_string(keywords[i]);
        while(std::getline(stream_string, value, dilimeter))
        {
            splited_cont.push_back(value);
        }
        i++;
    }
    
    return splited_cont;
}

std::map<std::string, std::string>  parseCookies(std::string header)
{
    std::string value;
    Session cookie;
    // std::map<std::string, std::string> cookie;
    std::vector<std::string> splited_by_space;
    std::vector<std::string> splited_by_semicolon;
    std::vector<std::string> splited_by_equal_sign;
    std::istringstream strin(header);
    size_t i = 0;

    while(std::getline(strin, value, ' '))
        splited_by_space.push_back(value);

    splited_by_semicolon = split(splited_by_space, ';');
    splited_by_equal_sign = split(splited_by_semicolon, '=');

    while(i + 1 < splited_by_equal_sign.size())
    {
        // cookie.insert(std::make_pair(splited_by_equal_sign[i], splited_by_equal_sign[i + 1]));
        cookie.SetSessionVal(splited_by_equal_sign[i], splited_by_equal_sign[i + 1]);
        i += 2;
    }
    return cookie.GetData();
}