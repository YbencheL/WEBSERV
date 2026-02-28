#include "../includes/ConfigPars.hpp"


void handle_client_mbs(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)keyword;
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> loc.client_max_body_size;
        if(ss.fail() || !ss.eof())
            error_line(": client_max_body_size must be a number", tokenContainer[i].line);
        countARG = 0;
    }else
        error_line(": client_max_body_size must only have one argument", tokenContainer[i].line);
}

void handle_allow_methods(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)countARG;
    (void)keyword;
    std::vector<std::string> non_duplicated_keyword;
    non_duplicated_keyword.push_back("GET");
    non_duplicated_keyword.push_back("POST");
    non_duplicated_keyword.push_back("DELETE");

    i++;
    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
    {
        if (tokenContainer[i].value == "GET" || tokenContainer[i].value == "POST"
            || tokenContainer[i].value == "DELETE")
        {
        for(size_t i = 0; i < non_duplicated_keyword.size(); i++)
            duplicate_check(loc.allow_methods, non_duplicated_keyword[i]);
                loc.allow_methods.push_back(tokenContainer[i].value);
        }
        else
            error_line(": only allowed methods are (GET, POST, DELETE)", tokenContainer[i].line);
        i++;
    }
}

void handle_index(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)countARG;
    (void)keyword;
    i++;

    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
    {
        loc.index.push_back(tokenContainer[i].value);
        i++;
    }
}

// work on it
void handle_redirections(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    countARG = 0;
    std::deque<int> errorsnum;
    std::string value;
    int errornum = 0;
    int num = 0;

    i++;
    while(tokenContainer[i].value != ";")
    {
        errornum = 0;
        std::stringstream ss(tokenContainer[i].value);
        ss >> errornum;
        if (ss.fail() || !ss.eof())
        {
            if (!value.empty())
                error_line(": there must be only one path in a directive or none", tokenContainer[i].line);
            else
                value = tokenContainer[i].value;
        }
        else
        {
            num = errornum;
            if ((errornum >= 100 && errornum < 600))
                errorsnum.push_back(errornum);
            else
                error_line(": directive number must be a valid http number", tokenContainer[i].line);
        }
        i++;
    }
    if (errorsnum.empty())
        error_line(": directive is missing a error number", tokenContainer[i].line);
    else
    {
        if (keyword == "return")
        {
            if (errorsnum.size() > 1)
                error_line(": more then one status code in return", tokenContainer[i].line);
            loc.redirection.insert(std::make_pair(errorsnum, value));
        }else
        {
            countARG = std::count(errorsnum.begin(), errorsnum.end(), num);
            if (countARG > 1)
                error_line(": duplicate status code in error_page", tokenContainer[i].line);
            loc.error_page.insert(std::make_pair(errorsnum, value));
        }
    }
}

void handle_cgi(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)countARG;
    (void)keyword;

    if (tokenContainer[i].value == "cgi_extension")
    {
        i++;
        while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
        {
            loc.cgi_extension.push_back(tokenContainer[i].value);
            i++;
        }
    }else if (tokenContainer[i].value == "cgi_path")
    {
        i++;
        while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
        {
            loc.cgi_path.push_back(tokenContainer[i].value);
            i++;
        }
    }
}

void handle_autoindex(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)keyword;

    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "off")
            loc.autoindex = false;
        else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "on")
            loc.autoindex = true;
        else
            error_line(": autoindex works with only on or off options", tokenContainer[i].line);
        countARG = 0;
    }else
        error_line(": autoindex must only have one argument", tokenContainer[i].line);
}
