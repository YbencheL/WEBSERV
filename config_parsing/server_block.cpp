#include "ConfigPars.hpp"

void handle_listen(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i)
{
    int port = 0;

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

void handle_host(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i)
{
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

void handle_server_name(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i)
{
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

void handle_server_block_index(std::deque<Token>& tokenContainer, ServerBlock& Serv, ssize_t& i)
{
    i++;
    if (Serv.index.empty())
    {
        while(tokenContainer[i].type == 1)
        {
            Serv.index.push_back(tokenContainer[i].value);
            i++;
        }
    }
}

void handle_error_page(std::deque<Token>& tokenContainer, ServerBlock& Serv, ssize_t& i)
{
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
            std::map<std::deque<int>,std::string>::iterator it = Serv.error_page.find(errorsnum);
            if (it != Serv.error_page.end())
                Serv.error_page.erase(it);
        }
        Serv.error_page.insert(std::make_pair(errorsnum, value));
}

void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i)
{
    int countARG = 0;

    if (tokenContainer[i].value == "listen")
        handle_listen(tokenContainer, Serv, countARG, i);
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "host")
        handle_host(tokenContainer, Serv, countARG, i);
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "root")
        handle_server_block_root(tokenContainer, Serv, countARG, i, insideLoc);
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "server_name")
        handle_server_name(tokenContainer, Serv, countARG, i);
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "client_max_body_size")
        handle_server_block_client_mbs(tokenContainer, Serv, countARG, i, insideLoc);
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "error_page")
        handle_error_page(tokenContainer, Serv, i);
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "index")
        handle_server_block_index(tokenContainer, Serv, i);
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