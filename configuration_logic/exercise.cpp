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
    std::string path;
    std::string root;
    std::deque<std::string> index;
    std::deque<std::string> allow_methods;
    bool autoindex;
    std::deque<std::string> cgi_extention;
    std::deque<std::string> cgi_path;
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
    std::deque<LocationBlock> locations;
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
                if (tokenContainer[i + 1].type != 0)
                {
                    if (tokenContainer[i + 1].value == "}")
                        error_line(": sytanx error related to '}'", tokenContainer[i + 1].line);
                    else
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
                if ((i - 1) >= 0 && tokenContainer[i - 2].value == ";")
                    error_line(": syntax error related to ;", tokenContainer[i].line);
                else if (tokenContainer[i + 1].type == 1)
                    error_line(": unkown keyword", tokenContainer[i + 1].line);
            }
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

    for (int i = 0; i < keywords.size(); i++)
    {
        if (keywords[i] == name)
            count++;
        else if (keywords[i] == "server")
            count = 0;
        if (count > 1)
            throw std::runtime_error("ERROR: there must be no duplicates for these keywords (listen client_max_body_size server_name error_page)");
    }
}

void extracting_server_config(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs)
{
    // bool insideServerBlock = false;
    ServerBlock Serv;
    std::deque<std::string> keywords;
    size_t keepCountOfBrase = 0;

    // init
    Serv.listen = 0;
    Serv.client_max_body_size = 0;
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
        }else if (tokenContainer[i].value == "{")
            keepCountOfBrase++;
        else if (tokenContainer[i].value == "}" && keepCountOfBrase)
        {
            keepCountOfBrase--;
            if (keepCountOfBrase == 0)
                ServerConfigs.push_back(Serv);
        }
    }

}

void extracting_location_config(std::deque<Token>& tokenContainer , ServerBlock& Serv)
{
    bool InsideLocationBlock = false;
    bool SetRoot = false;
    bool SetAutoindex = false;
    bool SetIndexfile = false;
    size_t pos = 0;
    
    for (int i = 0; i < tokenContainer.size(); i++)
    {
        LocationBlock loc;
        loc.autoindex = false;
        if (!InsideLocationBlock && (tokenContainer[i].value == "cgi_extension" || tokenContainer[i].value == "cgi_path"))
            error_line(": unkown keyword", tokenContainer[i].line);
        if ((i - 2) >= 0 && tokenContainer[i - 2].value == "location" && tokenContainer[i].value == "{")
        {
            i++;
            InsideLocationBlock = true;
            SetRoot = false;
            SetAutoindex = false;
            SetIndexfile = false;
            if ((pos = tokenContainer[i].value.find_first_of("/")) != 0 && tokenContainer[i + 1].value == "{")
                error_line(": paths must start with /", tokenContainer[i].line);
            else if ((i - 1) >= 0 && tokenContainer[i].type == 1 && tokenContainer[i - 1].value == "location")
                loc.path = tokenContainer[i].value;
            else if (tokenContainer[i].value == "root")
            {
                loc.root = tokenContainer[i + 1].value;
                SetRoot = true;
            }
            else if (tokenContainer[i].value == "index")
            {
                i++;
                while(tokenContainer[i].type == 1)
                {
                    loc.index.push_back(tokenContainer[i].value);
                    i++;
                }
                SetIndexfile = true;
            }else if (tokenContainer[i].value == "allow_methods")
            {
                i++;
                while(tokenContainer[i].type == 1)
                {
                    if (tokenContainer[i].value == "GET" || tokenContainer[i].value == "POST" || tokenContainer[i].value == "DELETE")
                        loc.index.push_back(tokenContainer[i].value);
                    else
                        error_line(": only allowed methods are (GET, POST, DELETE)", tokenContainer[i].line);
                    i++;
                }
            }else if (tokenContainer[i].value == "autoindex")
            {
                if (tokenContainer[i + 1].value == "off")
                    loc.autoindex = false;
                else if (tokenContainer[i + 1].value == "on")
                    loc.autoindex = true;
                else
                    error_line(": auto index works with only on or off options", tokenContainer[i].line);
                SetAutoindex = true;
            }else if(tokenContainer[i].value == "cgi_pass")
            {
                i++;
                while(tokenContainer[i].type == 1)
                {
                    loc.allow_methods.push_back(tokenContainer[i].value);
                    i++;
                }
            }else if(tokenContainer[i].value == "cgi_extention")
            {
                i++;
                while(tokenContainer[i].type == 1)
                {
                    loc.cgi_extention.push_back(tokenContainer[i].value);
                    i++;
                }
            }else if (tokenContainer[i].value == "cgi_pass")
            {
                i++;
                while(tokenContainer[i].type == 1)
                {
                    loc.cgi_path.push_back(tokenContainer[i].value);
                    i++;
                } 
            }
        }
        else if (tokenContainer[i].value == "}")
        {
            InsideLocationBlock = false;
            Serv.locations.push_back(loc);
        }   
    }
}

void checking_for_defaults(ServerBlock& Serv)
{
    if ((Serv.listen < 0 || Serv.listen > 65535))
        throw std::runtime_error("ERROR: port has incorrect value");
    else if (Serv.client_max_body_size < 0)
        throw std::runtime_error("ERROR: client_max_body_size has incorrect value");
    else if (!Serv.listen || Serv.root.empty() || Serv.error_page.empty())
        throw std::runtime_error("ERROR: missing value (root, listen, error_page)");
    if (Serv.host.empty()) Serv.host = "127.0.0.1";
    if (Serv.server_name.empty()) Serv.server_name = "WEBSERV_42";
    if (Serv.index.empty()) Serv.index.push_back("index.html");
    if (!Serv.client_max_body_size) Serv.client_max_body_size = 3000000;
    for (int i = 0; i < Serv.locations.size(); i++)
    {
        if (Serv.locations[i].root.empty()) Serv.locations[i].root = Serv.root;
        if (Serv.locations[i].index.empty()) Serv.locations[i].index = Serv.index;
        if (Serv.locations[i].allow_methods.empty())
        {
            Serv.locations[i].allow_methods.push_back("GET");
            Serv.locations[i].allow_methods.push_back("POST");
            Serv.locations[i].allow_methods.push_back("DELETE");
        }
    }
}   

void tokenzation(std::string fileContent)
{
    std::string tok;
    size_t Line;
    size_t pos;
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
    extracting_server_config(tokenContainer, serverConfigs);
    for (int i = 0; i < serverConfigs.size(); i++)
    {
        extracting_location_config(tokenContainer, serverConfigs[i]);
        checking_for_defaults(serverConfigs[i]);
    }
    // debugging code
    // for(size_t i = 0; i < tokenContainer.size(); i++)
    //     std::cout << tokenContainer[i].value << std::endl;
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