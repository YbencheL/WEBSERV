#include "ConfigPars.hpp"

void handle_listen(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    int port = 0;
    (void)insideLoc;

    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> port;
        if (ss.fail() || !ss.eof())
            error_line(": listen must only have a valid port number", tokenContainer[i].line);
        Serv.listen = port;
        port = 0;
        countARG = 0;
    }else
        error_line(": listen must only have one argument", tokenContainer[i].line);
}

void handle_timeout(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    int sec = 0;
    (void)insideLoc;

    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> sec;
        if (ss.fail() || !ss.eof())
            error_line(": set_timeout must only have a valid number", tokenContainer[i].line);
        Serv.set_timeout = sec;
        sec = 0;
        countARG = 0;
    }else
        error_line(": set_timeout must only have one argument", tokenContainer[i].line);
}

void handle_host(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    (void)insideLoc;
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        Serv.host = tokenContainer[i].value;
        countARG = 0;
    }else
        error_line(": host must only have one argument", tokenContainer[i].line);
}

void handle_server_block_root(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1 && !insideLoc)
    {
        Serv.root = tokenContainer[i].value;
        countARG = 0;
    }else if (countARG > 1)
        error_line(": root must only have one argument", tokenContainer[i].line);
    countARG = 0;
}

void handle_server_name(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc)
{
    (void)insideLoc;
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        Serv.server_name = tokenContainer[i].value;
        countARG = 0;
    }else
        error_line(": server_name must only have one argument", tokenContainer[i].line);
}

void handle_server_block_client_mbs(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1 && !insideLoc)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> Serv.client_max_body_size;
        if(ss.fail() || !ss.eof())
            error_line(": client_max_body_size must be a number", tokenContainer[i].line);
        countARG = 0;
    }else if (countARG > 1)
        error_line(": client_max_body_size must only have one argument", tokenContainer[i].line);
}

void handle_server_block_index(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    (void)countARG;
    (void)insideLoc;
    i++;
    if (Serv.index.empty() && !insideLoc)
    {
        while(tokenContainer[i].type == 1)
        {
            Serv.index.push_back(tokenContainer[i].value);
            i++;
        }
    }
}

void handle_error_page_server(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    (void)countARG;
    std::deque<int> errorsnum;
    std::string value;
    int errornum = 0;
    int num = errornum;
    i++;
    while(tokenContainer[i].value != ";")
    {
        errornum = 0;
        std::stringstream ss(tokenContainer[i].value);
        ss >> errornum;
        if (ss.fail() || !ss.eof())
        {
            if (!value.empty())
                error_line(": there must be only one path in erro_page or none", tokenContainer[i].line);
            else
                value = tokenContainer[i].value;
        }
        else
        {
            num = errornum;
            if ((errornum >= 100 && errornum < 600))
                errorsnum.push_back(errornum);
            else
                error_line(": error page number must be a valid http number", tokenContainer[i].line);
        }
        i++;
    }
    if (errorsnum.empty())
        error_line(": error_page is missing a page error number", tokenContainer[i].line);
    else
    {
        countARG = std::count(errorsnum.begin(), errorsnum.end(), num);
        if (countARG > 1)
            error_line(": duplicate status code in error_page", tokenContainer[i].line);
    }
    if (!insideLoc)
        Serv.error_page.insert(std::make_pair(errorsnum, value));
}

typedef void(*handler)(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc);

void handler_caller(std::map<std::string, handler>& handler_map)
{
    handler_map["listen"] = &handle_listen;
    handler_map["set_timeout"] = &handle_timeout;
    handler_map["host"] = &handle_host;
    handler_map["root"] = &handle_server_block_root;
    handler_map["client_max_body_size"] = &handle_server_block_client_mbs;
    handler_map["server_name"] = &handle_server_name;
    handler_map["error_page"] = &handle_error_page_server;
    handler_map["index"] = &handle_server_block_index;
}

void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i)
{
    int countARG = 0;
    std::map<std::string, handler> handler_map;

    handler_caller(handler_map);
    if (handler_map.find(tokenContainer[i].value) != handler_map.end())
        handler_map[tokenContainer[i].value](tokenContainer, Serv, countARG, i, insideLoc);
    else if (tokenContainer[i].value == "location")
        insideLoc = true;
    else if (insideLoc && (tokenContainer[i].value == "listen" || tokenContainer[i].value == "server_name" || tokenContainer[i].value == "error_page"))
            error_line(": listen, server_name, client_mbs and error_pages must be inside server block not location", tokenContainer[i].line);
    else if (!insideLoc && (tokenContainer[i].value == "autoindex" || tokenContainer[i].value == "return" || tokenContainer[i].value == "cgi_path" ||
            tokenContainer[i].value == "cgi_extention" || tokenContainer[i].value == "allow_methods"))
            error_line(": location only keyword inside server block", tokenContainer[i].line);
}

void extracting_server_blocks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs)
{
    ServerBlock Serv;
    ssize_t keepCountOfBrase = 0;
    bool insideLoc = false;

    // init
    Serv.client_max_body_size = 0;
    Serv.listen = 0;
    Serv.locations.clear();
    Serv.error_page.clear();
    Serv.index.clear();
    // duplicate check rule
    checking_for_keyword_dups(tokenContainer);
    // storing values
    for (ssize_t i = 0; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
            extracting_values_from_server_block(tokenContainer, insideLoc, Serv, i);
        else if (tokenContainer[i].value == "{")
            keepCountOfBrase++;
        else if (tokenContainer[i].value == "}" && keepCountOfBrase)
        {
            keepCountOfBrase--;
            if (keepCountOfBrase == 0)
            {
                ServerConfigs.push_back(Serv);
                Serv = ServerBlock();
            }else
                insideLoc = false;
        }
    }

}