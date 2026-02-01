#include "ConfigPars.hpp"

void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i)
{
    int countARG = 0;
    int port = 0;

    if (tokenContainer[i].value == "listen")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            std::stringstream ss(tokenContainer[i].value);
            ss >> port;
            if (ss.fail())
                error_line(": listen must only have a valid port number", tokenContainer[i].line);
            Serv.listen.insert(port);
            port = 0;
            countARG = 0;
        }else
            error_line(": listen must only have one argument", tokenContainer[i].line);
    }
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "host")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            Serv.host.insert(tokenContainer[i].value);
            countARG = 0;
        }else
            error_line(": host must only have one argument", tokenContainer[i].line);
    }
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "root")
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
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "server_name")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            Serv.server_name.insert(tokenContainer[i].value);
            countARG = 0;
        }else
            error_line(": server_name must only have one argument", tokenContainer[i].line);
    }
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "client_max_body_size")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            std::stringstream ss(tokenContainer[i].value);
            ss >> Serv.client_max_body_size;
            countARG = 0;
        }else
            error_line(": client_max_body_size must only have one argument", tokenContainer[i].line);
    }
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "error_page")
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
            if (ss.fail())
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
    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "index")
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
    }else if (tokenContainer[i].value == "location")
        insideLoc = true;
    else if (insideLoc && (tokenContainer[i].value == "listen" || tokenContainer[i].value == "server_name" || tokenContainer[i].value == "error_page" ||
            tokenContainer[i].value == "client_max_body_size"))
            error_line(": listen, server_name, client_mbs and error_pages must be inside server block not location", tokenContainer[i].line);
}

void extracting_server_blocks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs)
{
    ServerBlock Serv;
    ssize_t keepCountOfBrase = 0;
    bool insideLoc = false;

    // init
    Serv.client_max_body_size = 0;
    Serv.locations.clear();
    Serv.listen.clear();
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