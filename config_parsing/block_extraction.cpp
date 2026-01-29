#include "ConfigPars.hpp"

void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, size_t& i)
{
    int countARG = 0;

    if (tokenContainer[i].value == "listen")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            std::stringstream ss(tokenContainer[i].value);
            ss >> Serv.listen;
            countARG = 0;
        }else
            error_line(": listen must only have one argument", tokenContainer[i].line);
    }
    else if (i < tokenContainer.size() && tokenContainer[i].value == "host")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            Serv.host = tokenContainer[i].value;
            countARG = 0;
        }else
            error_line(": host must only have one argument", tokenContainer[i].line);
    }
    else if (i < tokenContainer.size() && tokenContainer[i].value == "root")
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
    else if (i < tokenContainer.size() && tokenContainer[i].value == "server_name")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 1)
        {
            Serv.server_name = tokenContainer[i].value;
            countARG = 0;
        }else
            error_line(": server_name must only have one argument", tokenContainer[i].line);
    }
    else if (i < tokenContainer.size() && tokenContainer[i].value == "client_max_body_size")
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
    else if (i < tokenContainer.size() && tokenContainer[i].value == "error_page")
    {
        countARG = count_to_symbol(tokenContainer, i, countARG);
        if (countARG == 2)
        {
            i--;
            int errornum = 0;
            std::stringstream ss(tokenContainer[i].value);
            ss >> errornum;
            std::pair<std::map<int,std::string>::iterator, bool> isdup;
            isdup = Serv.error_page.insert(std::make_pair(errornum, tokenContainer[i + 1].value));
            if (!isdup.second)
                throw std::runtime_error("ERROR: Duplicated error page number in one server block");
            countARG = 0;
        }else
            error_line(": error_page must have two values error num and path", tokenContainer[i].line);
    }
    else if (i < tokenContainer.size() && tokenContainer[i].value == "index")
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
    size_t keepCountOfBrase = 0;
    bool insideLoc = false;

    // init
    Serv.listen = 0;
    Serv.client_max_body_size = 0;
    Serv.locations.clear();
    Serv.error_page.clear();
    Serv.index.clear();
    // duplicate check rule
    checking_for_keyword_dups(tokenContainer);
    // storing values
    for (size_t i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
        {
            extracting_values_from_server_block(tokenContainer, insideLoc, Serv, i);
        }else if (tokenContainer[i].value == "{")
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

void extracting_location_blocks(std::deque<Token>& tokenContainer , ServerBlock& Serv, size_t& i)
{
    bool InsideLocationBlock = false;
    size_t keepCountOfBrase = 0;
    size_t pos = 0;
    
    for (; i < tokenContainer.size(); i++)
    {
        if (!InsideLocationBlock && (tokenContainer[i].value == "cgi_extension" || tokenContainer[i].value == "cgi_path"))
            error_line(": unkown keyword", tokenContainer[i].line);
        if (tokenContainer[i].value == "location")
        {
            LocationBlock loc;
            loc.autoindex = false;
            i++;
            InsideLocationBlock = true;
            while (InsideLocationBlock)
            {
                if (tokenContainer[i].value == "{")
                    keepCountOfBrase++;
                else if ((i + 1) < tokenContainer.size() && (pos = tokenContainer[i].value.find_first_of("/")) != 0 && tokenContainer[i + 1].value == "{")
                    error_line(": paths must start with /", tokenContainer[i].line);
                else if ((i - 1) >= 0 && tokenContainer[i].type == 1 && tokenContainer[i - 1].value == "location")
                    loc.path = tokenContainer[i].value;
                else if (i < tokenContainer.size() && tokenContainer[i].value == "root")
                    loc.root = tokenContainer[i + 1].value;
                else if (i < tokenContainer.size() && tokenContainer[i].value == "index")
                {
                    i++;
                    while(i < tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        loc.index.push_back(tokenContainer[i].value);
                        i++;
                    }
                }else if (i < tokenContainer.size() && tokenContainer[i].value == "allow_methods")
                {
                    i++;
                    while(i < tokenContainer.size() && tokenContainer[i].type == 1)
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
                }else if (i < tokenContainer.size() && tokenContainer[i].value == "autoindex")
                {
                    if (i < tokenContainer.size() && tokenContainer[i + 1].value == "off")
                        loc.autoindex = false;
                    else if (i < tokenContainer.size() && tokenContainer[i + 1].value == "on")
                        loc.autoindex = true;
                    else
                        error_line(": auto index works with only on or off options", tokenContainer[i].line);
                }else if(i < tokenContainer.size() && tokenContainer[i].value == "cgi_extension")
                {
                    i++;
                    while(i < tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        loc.cgi_extension.push_back(tokenContainer[i].value);
                        i++;
                    }
                }else if (i < tokenContainer.size() && tokenContainer[i].value == "cgi_path")
                {
                    i++;
                    while(i < tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        loc.cgi_path.push_back(tokenContainer[i].value);
                        i++;
                    } 
                }
                else if (tokenContainer[i].value == "}")
                {
                    keepCountOfBrase--;
                    Serv.locations.push_back(loc);
                    InsideLocationBlock = false;
                    break;
                }else if (tokenContainer[i].value == "listen" || tokenContainer[i].value == "server_name" || tokenContainer[i].value == "error_page" ||
                    tokenContainer[i].value == "client_max_body_size")
                    error_line(": listen, server_name, client_mbs and error_pages must be inside server block not location", tokenContainer[i].line);
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

void extracting_blocks_plus_final_checks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& serverConfigs)
{
    size_t indx = 0;
    std::string msg;
    std::multimap<int, std::string> seenPortAndIp;
    std::multimap<int, std::string> seenPortAndName;

    extracting_server_blocks(tokenContainer, serverConfigs);
    for (size_t i = 0; i < serverConfigs.size(); i++)
    {
        extracting_location_blocks(tokenContainer, serverConfigs[i], indx);
        checking_for_defaults(serverConfigs[i]);
        seenPortAndIp.insert(std::make_pair(serverConfigs[i].listen, serverConfigs[i].host));
        seenPortAndName.insert(std::make_pair(serverConfigs[i].listen, serverConfigs[i].server_name));
        if (serverConfigs[i].locations.empty() && serverConfigs[i].root.empty())
            throw std::runtime_error("ERROR: missing value (root)");
    }
    // check for server blocks with same ip and port
    msg = "ERROR: more then a server block has the same port and ip address";
    checking_for_virtual_hosts(seenPortAndIp, msg);
    // check for server block with same port and server name
    msg = "ERROR: more then a server block has the same port and server name";
    checking_for_virtual_hosts(seenPortAndName, msg);
}

void tokenzation(std::string fileContent)
{
    std::string tok;
    size_t Line;
    // size_t pos;
    std::deque<Token> tokenContainer;
    std::deque<ServerBlock> serverConfigs;

    Line = 1;
    for(size_t i = 0; i < fileContent.size(); i++)
    {
        if (fileContent[i] == '#')
        {
            while(fileContent[i] != '\n')
                i++;
        }
        if (fileContent[i] == '\n')
        {
            if (!tok.empty())
                identifying_words_and_keywords(tok, tokenContainer, Line);
            Line++;
        }
        else if ((fileContent[i] == ' ' || fileContent[i] == '\t'))
        {
            if (!tok.empty())
                identifying_words_and_keywords(tok, tokenContainer, Line);
        }else if ((fileContent[i] == ';' || fileContent[i] == '{' || fileContent[i] == '}'))
        {
            if (!tok.empty())
                identifying_words_and_keywords(tok, tokenContainer, Line);
            Token tikken;
            tikken.type = SYMBOL;
            tikken.value.push_back(fileContent[i]);
            tikken.line = Line;
            tokenContainer.push_back(tikken);
            tok.clear();
        }
        else
            tok.push_back(fileContent[i]);
    }
    if (!tok.empty())
        identifying_words_and_keywords(tok, tokenContainer, Line);
    is_syntax_valid(tokenContainer);
    extracting_blocks_plus_final_checks(tokenContainer, serverConfigs);
    // debugging code
    debugging(serverConfigs);
}