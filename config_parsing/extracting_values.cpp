#include "ConfigPars.hpp"

void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line)
{
    if (tok == "server" || tok == "location" || tok == "listen" || tok == "host" || tok == "server_name"
        || tok == "root" || tok == "index" || tok == "allow_methods" || tok == "autoindex" || tok == "return"
            || tok == "cgi_extension" || tok == "cgi_path" || tok == "error_page" || tok == "client_max_body_size")
    {
        Token tikken;
        tikken.type = KEYWORD;
        tikken.value.append(tok);
        tikken.line = Line;
        tokenContainer.push_back(tikken);
        tok.clear();
    }else
    {
        Token tikken;
        tikken.type = WORD;
        tikken.value.append(tok);
        tikken.line = Line;
        tokenContainer.push_back(tikken);
        tok.clear();
    }
}


void extracting_blocks_plus_final_checks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& serverConfigs)
{
    ssize_t indx = 0;
    std::string msg;
    std::deque<int> seenPort;

    extracting_server_blocks(tokenContainer, serverConfigs);
    for (ssize_t i = 0; i < (ssize_t)serverConfigs.size(); i++)
    {
        extracting_location_blocks(tokenContainer, serverConfigs[i], indx);
        checking_values(serverConfigs[i]);
        seenPort.push_back(serverConfigs[i].listen);
        if (serverConfigs[i].locations.empty() && serverConfigs[i].root.empty())
            throw std::runtime_error("ERROR: missing value (root)");
    }
    // check for server blocks with same ip and port
    checking_for_virtual_hosts(seenPort);
}

std::deque<ServerBlock> tokenzation(std::string fileContent)
{
    std::string tok;
    ssize_t Line;
    // ssize_t pos;
    std::deque<Token> tokenContainer;
    std::deque<ServerBlock> serverConfigs;

    Line = 1;
    for(ssize_t i = 0; i < (ssize_t)fileContent.size(); i++)
    {
        if (fileContent[i] == '#')
        {
            while((i <= (ssize_t)fileContent.size()) && fileContent[i] != '\n')
                i++;
        }
        else if (fileContent[i] == '\n')
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
    if (tokenContainer.empty())
        throw std::runtime_error("ERROR: nothing was provided in the config file");
    is_syntax_valid(tokenContainer);
    extracting_blocks_plus_final_checks(tokenContainer, serverConfigs);
    return serverConfigs;
}