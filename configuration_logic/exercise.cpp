#include  <iostream>
#include  <fstream>
#include  <deque>
#include <cctype>
#include <sstream>
#include <map>
#include <algorithm>

enum TokenType {
    KEYWORD,
    WORD,
    SYMBOL
};

struct Token
{
    TokenType type;
    std::string value;
    int line; 
};

struct LocationBlock
{

};

struct ServerBlock
{
    int listen;
    std::string root;
    std::string host;
    std::string server_name;
    int client_max_body_size;
    std::deque<std::string> index;
    std::map<int, std::string> error_page;
    // std::deque<LocationBlock> locations;
};

void error_line(std::string msg, int Line)
{
    std::string errorLine;
    std::stringstream ss;

    ss << Line;
    errorLine = "ERROR on line " + ss.str() + msg;
    throw std::runtime_error(errorLine);
}

void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line)
{
    if (tok == "server" || tok == "location" || tok == "listen" || tok == "host"
        || tok == "root" || tok == "index" || tok == "allow_methods" || tok == "autoindex"
            || tok == "cgi_pass" || tok == "error_page" || tok == "client_max_body_size")
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
    size_t keepCountOfBrase = 0;
    size_t ServerBlockCount = 0;
    size_t LocationBlockCount = 0;
    bool insideServer = false;

    for (int i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 2)
        {
            if (tokenContainer[i].value == "{")
            {
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
            }else if ((i - 1) >= 0 && tokenContainer[i].value == ";" && tokenContainer[i - 2].value == ";" )
                error_line(": syntax error related to ;", tokenContainer[i].line);
        }
        else if (tokenContainer[i].type == 1)
        {
            if ((i - 1) >= 0 && tokenContainer[i - 1].value != "location" && tokenContainer[i + 1].type != 1 && tokenContainer[i + 1].value != ";")
                error_line(": directives must end with ;", tokenContainer[i].line);
            if (!insideServer)
                error_line(": configuration must be done inside a server block", tokenContainer[i].line);
        }
        else if (tokenContainer[i].type == 0)
        {

            if (tokenContainer[i].value == "server")
                ServerBlockCount++;
            else if (tokenContainer[i].value == "location")
                LocationBlockCount++;
                
            if ((tokenContainer[i].value == "server" && tokenContainer[i + 1].value != "{") ||
                    (tokenContainer[i].value == "location" && tokenContainer[i + 2].value != "{"))
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

void duplicate_check(std::deque<std::string>& keywords, std::string name)
{
    int count = 0;
    
    count = std::count(keywords.begin(), keywords.end(), name);
    if (count > 1)
        throw std::runtime_error("ERROR: there must be no duplicates for these keywords (listen client_max_body_size server_name error_page)");
}

void extracting_server_config(std::deque<Token>& tokenContainer)
{
    // bool insideServerBlock = false;
    ServerBlock Serv;
    std::deque<std::string> keywords;

    // duplicate check rule
    for (int i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
            keywords.push_back(tokenContainer[i].value);
    }
    duplicate_check(keywords, "listen");
    duplicate_check(keywords, "client_max_body_size");
    duplicate_check(keywords, "server_name");
    duplicate_check(keywords, "error_page");
    // storing values
    for (int i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
        {
            if (tokenContainer[i].value == "listen")
            {
                std::stringstream ss(tokenContainer[i + 1].value);
                ss >> Serv.listen;
            }
            else if (tokenContainer[i].value == "host")
                Serv.host = tokenContainer[i + 1].value;
            else if (tokenContainer[i].value == "root")
                Serv.root = tokenContainer[i + 1].value;
            else if (tokenContainer[i].value == "server_name")
                Serv.server_name = tokenContainer[i + 1].value;
            else if (tokenContainer[i].value == "client_max_body_size")
            {
                std::stringstream ss(tokenContainer[i + 1].value);
                ss >> Serv.client_max_body_size;
            }
            else if (tokenContainer[i].value == "error_page")
            {
                int errornum = 0;
                std::stringstream ss(tokenContainer[i + 1].value);
                ss >> errornum;
                Serv.error_page.insert(std::make_pair(errornum, tokenContainer[i + 2].value));
            }
            else if (tokenContainer[i].value == "index")
            {
                i++;
                while(tokenContainer[i].type == 1)
                {
                    Serv.index.push_back(tokenContainer[i].value);
                    i++;
                }
            }
        }
    }
    if (Serv.server_name.empty())
        Serv.server_name = "WEBSERV";
    if (Serv.host.empty())
        Serv.host = "127.0.0.1";
    if (Serv.index.empty())
        Serv.index.push_back("index.html");
    if (!Serv.listen || Serv.root.empty() || Serv.error_page.empty()|| !Serv.client_max_body_size)
        throw std::runtime_error("ERROR: missing value!");
}

void extracting_location_config(std::deque<Token>& tokenContainer)
{

}

void tokenzation(std::string fileContent)
{
    std::string tok;
    size_t Line;
    size_t pos;
    std::deque<Token> tokenContainer;

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
    extracting_server_config(tokenContainer);
    extracting_location_config(tokenContainer);
    for(size_t i = 0; i < tokenContainer.size(); i++)
        std::cout << tokenContainer[i].value << std::endl;
}

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cout << "Error: file is missing" << std::endl;
        return 1;
    }
    std::ifstream infile(av[1]);
    if (infile.fail())
    {
        std::cout << "Error: can't open file!" << std::endl;
        return 1;
    }
    std::string fileContent((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    try
    {
        tokenzation(fileContent);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}