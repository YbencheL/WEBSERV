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

    i++;
    while(tokenContainer[i].value != ";")
    {
        if (tokenContainer[i].value == "GET" || tokenContainer[i].value == "POST"
            || tokenContainer[i].value == "DELETE")
            loc.allow_methods.insert(tokenContainer[i].value);
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

    while(tokenContainer[i].value != ";")
    {
        loc.index.insert(tokenContainer[i].value);
        i++;
    }
}

void handle_location_block_root(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)countARG;
    (void)keyword;
    i++;

    if (tokenContainer[i].value != ";")
        loc.root = tokenContainer[i].value;
    else
        error_line(": root value is missing", tokenContainer[i].line);
}

// work on it
void handle_redirections(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    countARG = 0;
    std::string value;
    std::set<int> errorsnum;
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
            if ((errornum >= 100 && errornum < 600))
            {
                num = errornum;
                errorsnum.insert(num);
            }
            else
                error_line(": directive number must be a valid http number", tokenContainer[i].line);
        }
        i++;
    }
    if (keyword == "return")
    {
        if (errorsnum.size() == 1)
            loc.redirection[num] = value;
        else
            error_line(": there must not be more then one status code in return", tokenContainer[i].line);
    }
    else
    {
        for (std::set<int>::iterator it = errorsnum.begin();
            it != errorsnum.end(); ++it)
        {
            int code = *it;
            loc.error_page[code] = value;
        }
    }
}

void handle_cgi(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
    std::string& keyword)
{
    (void)keyword;
    countARG = 0;
    std::string key;
    std::string value;

    i++;
    while(tokenContainer[i].value != ";")
    {
        if (tokenContainer[i].value.find_first_of('.') == std::string::npos)
            error_line(": the extension is not valid must start with '.'", tokenContainer[i].line);
        key = tokenContainer[i].value;
        if (tokenContainer[i + 1].value != ";")
        {
            i++;
            value = tokenContainer[i].value;
            if (!is_cgi_path_valid(value))
                error_line(": the cgi path in not valid", tokenContainer[i].line);
        }else
            error_line(": the cgi path in not valid", tokenContainer[i].line);
        loc.cgi_handler.insert(std::make_pair(key, value));
        i++;
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
