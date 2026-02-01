#include "ConfigPars.hpp"

void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line)
{
    if (tok == "server" || tok == "location" || tok == "listen" || tok == "host" || tok == "server_name"
        || tok == "root" || tok == "index" || tok == "allow_methods" || tok == "autoindex"
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

void is_syntax_valid(std::deque<Token> tokenContainer)
{
    ssize_t keepCountOfBrase = 0;
    ssize_t ServerBlockCount = 0;
    ssize_t LocationBlockCount = 0;
    bool insideServer = false;

    for (ssize_t i = 0; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 2)
        {
            if (tokenContainer[i].value == "{")
            {
                if (tokenContainer[i + 1].type != 0)
                {
                    if (tokenContainer[i + 1].value != "}")
                        error_line(": unkown keyword", tokenContainer[i + 1].line);
                }
                if ((i - 1) >= 0 && tokenContainer[i - 1].value == "server")
                    insideServer = true;
                keepCountOfBrase++;
            }
            else if (tokenContainer[i].value == "}" && keepCountOfBrase)
            {
                keepCountOfBrase--;
                if (keepCountOfBrase == 0)
                {
                    insideServer = false;
                    ServerBlockCount = 0;
                }
                LocationBlockCount = 0;
            }else if (tokenContainer[i].value == ";")
            {
                if ((i - 2) >= 0 && tokenContainer[i - 2].value == ";")
                    error_line(": syntax error related to ;", tokenContainer[i].line);
                else if (tokenContainer[i + 1].type == 1)
                    error_line(": unkown keyword", tokenContainer[i + 1].line);
            }
        }
        else if (tokenContainer[i].type == 1)
        {
            if ((i - 1) >= 0 && tokenContainer[i - 1].value != "location" && tokenContainer[i + 1].type != 1 && tokenContainer[i + 1].value != ";")
                error_line(": directives must end with ;", tokenContainer[i].line);
            else if (!insideServer)
                error_line(": configuration must be done inside a server block", tokenContainer[i].line);
        }
        else if (tokenContainer[i].type == 0)
        {

            if (tokenContainer[i].value == "server")
                ServerBlockCount++;
            else if (tokenContainer[i].value == "location")
                LocationBlockCount++;
                
            if (((tokenContainer[i].value == "server" && tokenContainer[i + 1].value != "{") ||
                    (tokenContainer[i].value == "location" && tokenContainer[i + 2].value != "{")))
                error_line(": server and location block must be followed with braces", tokenContainer[i].line);
            else if (tokenContainer[i].value == "location" && !insideServer)
                error_line(": location block must be inside the server block", tokenContainer[i].line);

            else if (ServerBlockCount > 1 || LocationBlockCount > 1)
                error_line(": nested server or location blocks isn't allowed!", tokenContainer[i].line);
        }
    }
    if (keepCountOfBrase != 0)
        throw std::runtime_error("ERROR: check brackets!");
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
        checking_for_defaults(serverConfigs[i]);
        seenPort.push_back(serverConfigs[i].listen);
        if (serverConfigs[i].locations.empty() && serverConfigs[i].root.empty())
            throw std::runtime_error("ERROR: missing value (root)");
    }
    // check for server blocks with same ip and port
    checking_for_virtual_hosts(seenPort);
}

void tokenzation(std::string fileContent)
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
    // debugging code
    debugging(serverConfigs);
}