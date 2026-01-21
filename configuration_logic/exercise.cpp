#include  <iostream>
#include  <fstream>
#include  <deque>
#include <cctype>
#include <sstream>

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

void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line)
    {
        if (tok == "server" || tok == "location" || tok == "listen"
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
    bool inside = false;
    std::string errorLine;
    std::stringstream ss;

    for (int i = 0; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 2)
        {
            if (tokenContainer[i].value == "{")
            {
                if ((i - 1) >= 0 && tokenContainer[i - 1].value == "server")
                    inside = true;
                keepCountOfBrase++;
            }
            else if (tokenContainer[i].value == "}" && keepCountOfBrase)
            {
                keepCountOfBrase--;
                if (keepCountOfBrase == 0)
                    inside = false;
            }
        }
        else if (tokenContainer[i].type == 1)
        {            
            if ((i - 1) >= 0 && tokenContainer[i - 1].value != "location" && tokenContainer[i + 1].value != ";")
            {
                ss << tokenContainer[i].line;
                errorLine = "ERROR on line " + ss.str() + ": directives must end with ;";
                throw std::runtime_error(errorLine);
            }
        }
        else if (tokenContainer[i].type == 0)
        {
            if (tokenContainer[i].value == "location" && !inside)
            {
                ss << tokenContainer[i].line;
                errorLine = "ERROR on line " + ss.str() + ": location block must be inside the server block";
                throw std::runtime_error(errorLine);
            }
        }
    }
    if (keepCountOfBrase != 0)
        throw std::runtime_error("ERROR: check brackets!");
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
        if (fileContent[i] == '\n')
        {
            if (!tok.empty())
                identifying_words_and_keywords(tok, tokenContainer, Line);
            Line++;
        }
        else if (fileContent[i] == ' ' || fileContent[i] == '\t')
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
    {
        if (tok == "server" || tok == "location")
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
    is_syntax_valid(tokenContainer);
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