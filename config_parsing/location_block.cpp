#include "ConfigPars.hpp"

void handle_client_mbs(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> loc.client_max_body_size;
        if(ss.fail() || !ss.eof())
            error_line(": client_max_body_size must be a number", tokenContainer[i].line);
        countARG = 0;
    }else
        error_line(": client_max_body_size must only have one argument", tokenContainer[i].line);
}

void handle_return(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        loc.redirection = tokenContainer[i].value;
        countARG = 0;
    }else
        error_line(": return must only have one argument", tokenContainer[i].line);
}

void handle_allow_methods(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    (void)countARG;
    i++;
    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
    {
        if (tokenContainer[i].value == "GET" || tokenContainer[i].value == "POST" || tokenContainer[i].value == "DELETE")
        {
            loc.allow_methods.push_back(tokenContainer[i].value);
            duplicate_check(loc.allow_methods, "GET");
            duplicate_check(loc.allow_methods, "POST");
            duplicate_check(loc.allow_methods, "DELETE");
        }
        else
            error_line(": only allowed methods are (GET, POST, DELETE)", tokenContainer[i].line);
        i++;
    }
}

void handle_index(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    (void)countARG;
    i++;
    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
    {
        loc.index.push_back(tokenContainer[i].value);
        i++;
    }
}

void handle_error_page(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    (void)countARG;
    std::deque<int> errorsnum;
    std::string value;
    int errornum = 0;
    i++;
    while(tokenContainer[i].value != ";")
    {
        errornum = 0;
        std::stringstream ss(tokenContainer[i].value);
        ss >> errornum;
        if (ss.fail() || !ss.eof())
        {
            if (!value.empty())
                error_line(": there must be only one path in erro_page", tokenContainer[i].line);
            else
                value = tokenContainer[i].value;
        }
        else
        {
            if ((errornum >= 100 && errornum < 600))
                errorsnum.push_back(errornum);
            else
                error_line(": error page number must be a valid http number", tokenContainer[i].line);
        }
        i++;
    }
    if (value.empty() || errorsnum.empty())
        error_line(": error_page is missing a path or a page error number", tokenContainer[i].line);
    else
    {
        std::map<std::deque<int>,std::string>::iterator it = loc.error_page.find(errorsnum);
        if (it != loc.error_page.end())
            loc.error_page.erase(it);
    }
    loc.error_page.insert(std::make_pair(errorsnum, value));
}

void handle_cgi(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    (void)countARG;
    if (tokenContainer[i].value == "cgi_extension")
    {
        i++;
        while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
        {
            loc.cgi_extension.push_back(tokenContainer[i].value);
            i++;
        }
    }else if (tokenContainer[i].value == "cgi_path")
    {
        i++;
        while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
        {
            loc.cgi_path.push_back(tokenContainer[i].value);
            i++;
        } 
    }
}

void handle_autoindex(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i)
{
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "off")
            loc.autoindex = false;
        else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "on")
            loc.autoindex = true;
        else
            error_line(": autoindex works with only on or off options", tokenContainer[i].line);
        countARG = 0;
    }else
        error_line(": autoindex must only have one argument", tokenContainer[i].line);
}

typedef void(*handler)(std::deque<Token>&, LocationBlock&, int, ssize_t&);

void handler_caller(std::map<std::string, handler>& handler_map)
{
    handler_map["client_max_body_size"] = &handle_client_mbs;
    handler_map["return"] = &handle_return;
    handler_map["index"] = &handle_index;
    handler_map["error_page"] = &handle_error_page;
    handler_map["allow_methods"] = &handle_allow_methods;
    handler_map["autoindex"] = &handle_autoindex;
    handler_map["cgi_extension"] = &handle_cgi;
    handler_map["cgi_path"] = &handle_cgi;
}

void extracting_location_blocks(std::deque<Token>& tokenContainer , ServerBlock& Serv, ssize_t& i)
{
    bool InsideLocationBlock = false;
    ssize_t keepCountOfBrase = 0;
    int countARG = 0;
    std::map<std::string, void(*)(std::deque<Token>&, LocationBlock&, int, ssize_t&)> handler_map;
    ssize_t pos = 0;

    handler_caller(handler_map);
    for (; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (!InsideLocationBlock && (tokenContainer[i].value == "cgi_extension" || tokenContainer[i].value == "cgi_path"))
        {
            std::cout << tokenContainer[i].value << std::endl;
            error_line(": unkown keyword", tokenContainer[i].line);
        }
        if (tokenContainer[i].value == "location")
        {
            LocationBlock loc;
            loc.client_max_body_size = 0;
            loc.autoindex = false;
            i++;
            InsideLocationBlock = true;
            while (InsideLocationBlock)
            {
                if (tokenContainer[i].value == "{")
                    keepCountOfBrase++;
                else if ((i + 1) < (ssize_t)tokenContainer.size() && (pos = tokenContainer[i].value.find_first_of("/")) != 0 && tokenContainer[i + 1].value == "{")
                    error_line(": paths must start with /", tokenContainer[i].line);
                else if ((i - 1) >= 0 && tokenContainer[i].type == 1 && tokenContainer[i - 1].value == "location")
                    loc.path = tokenContainer[i].value;
                else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "root")
                    loc.root = tokenContainer[i + 1].value;
                else if (handler_map.find(tokenContainer[i].value) != handler_map.end())
                    handler_map[tokenContainer[i].value](tokenContainer, loc, countARG, i);
                else if (tokenContainer[i].value == "}")
                {
                    keepCountOfBrase--;
                    Serv.locations.push_back(loc);
                    InsideLocationBlock = false;
                    break;
                }else if (tokenContainer[i].value == "listen" || tokenContainer[i].value == "server_name" ||
                        tokenContainer[i].value == "host")
                    error_line(": server only keyword inside location block", tokenContainer[i].line);
                i++;
            }
        }else if (tokenContainer[i].value == "{")
            keepCountOfBrase++;
        else if (tokenContainer[i].value == "}" && keepCountOfBrase)
        {
            keepCountOfBrase--;
            if (keepCountOfBrase == 0)
                break;
        }
    }           
}