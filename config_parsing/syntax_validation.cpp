#include "ConfigPars.hpp"

void keywords_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& LocationBlockCount, ssize_t& i, bool& insideServer)
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

void symbol_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& keepCountOfBrase, ssize_t& LocationBlockCount, ssize_t& i,
    bool& insideServer)
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

void is_syntax_valid(std::deque<Token> tokenContainer)
{
    ssize_t keepCountOfBrase = 0;
    ssize_t ServerBlockCount = 0;
    ssize_t LocationBlockCount = 0;
    bool insideServer = false;

    for (ssize_t i = 0; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == SYMBOL)
            symbol_validation(tokenContainer, ServerBlockCount, keepCountOfBrase, LocationBlockCount, i, insideServer);
        else if (tokenContainer[i].type == WORD)
        {
            if ((i - 1) >= 0 && tokenContainer[i - 1].value != "location" && tokenContainer[i + 1].type != 1 && tokenContainer[i + 1].value != ";")
                error_line(": directives must end with ;", tokenContainer[i].line);
            else if (!insideServer)
                error_line(": configuration must be done inside a server block", tokenContainer[i].line);
        }
        else if (tokenContainer[i].type == KEYWORD)
            keywords_validation(tokenContainer, ServerBlockCount, LocationBlockCount, i, insideServer);
    }
    if (keepCountOfBrase != 0)
        throw std::runtime_error("ERROR: check brackets!");
}