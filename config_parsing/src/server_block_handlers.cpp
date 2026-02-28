#include "../includes/ConfigPars.hpp"

void handle_listen(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    // size_t port = 0;
    (void)insideLoc;

    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> Serv.listen;
        if (ss.fail() || !ss.eof())
            error_line(": listen must only have a valid port number", tokenContainer[i].line);
        // Serv.listen = port;
        // port = 0;
        countARG = 0;
    }else
        error_line(": listen must only have one argument", tokenContainer[i].line);
}

void handle_timeout(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    // size_t sec = 0;
    (void)insideLoc;

    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> Serv.set_timeout;
        if (ss.fail() || !ss.eof())
            error_line(": set_timeout must only have a valid number", tokenContainer[i].line);
        // Serv.set_timeout = sec;
        // sec = 0;
        countARG = 0;
    }else
        error_line(": set_timeout must only have one argument", tokenContainer[i].line);
}

void handle_host(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    (void)insideLoc;
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        Serv.host = tokenContainer[i].value;
        countARG = 0;
    }else
        error_line(": host must only have one argument", tokenContainer[i].line);
}

void handle_server_block_root(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
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

void handle_server_name(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc)
{
    (void)insideLoc;
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1)
    {
        Serv.server_name = tokenContainer[i].value;
        countARG = 0;
    }else
        error_line(": server_name must only have one argument", tokenContainer[i].line);
}

void handle_server_block_client_mbs(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    countARG = count_to_symbol(tokenContainer, i, countARG);
    if (countARG == 1 && !insideLoc)
    {
        std::stringstream ss(tokenContainer[i].value);
        ss >> Serv.client_max_body_size;
        if(ss.fail() || !ss.eof())
            error_line(": client_max_body_size must be a number", tokenContainer[i].line);
        countARG = 0;
    }else if (countARG > 1)
        error_line(": client_max_body_size must only have one argument", tokenContainer[i].line);
}

void handle_server_block_index(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    (void)countARG;
    (void)insideLoc;
    i++;
    if (Serv.index.empty() && !insideLoc)
    {
        while(tokenContainer[i].type == 1)
        {
            Serv.index.push_back(tokenContainer[i].value);
            i++;
        }
    }
}

void handle_error_page_server(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc)
{
    (void)countARG;
    std::deque<int> errorsnum;
    std::string value;
    int errornum = 0;
    int num = errornum;
    i++;
    while(tokenContainer[i].value != ";")
    {
        errornum = 0;
        std::stringstream ss(tokenContainer[i].value);
        ss >> errornum;
        if (ss.fail() || !ss.eof())
        {
            if (!value.empty())
                error_line(": there must be only one path in erro_page or none", tokenContainer[i].line);
            else
                value = tokenContainer[i].value;
        }
        else
        {
            num = errornum;
            if ((errornum >= 100 && errornum < 600))
                errorsnum.push_back(errornum);
            else
                error_line(": error page number must be a valid http number", tokenContainer[i].line);
        }
        i++;
    }
    if (errorsnum.empty())
        error_line(": error_page is missing a page error number", tokenContainer[i].line);
    else
    {
        countARG = std::count(errorsnum.begin(), errorsnum.end(), num);
        if (countARG > 1)
            error_line(": duplicate status code in error_page", tokenContainer[i].line);
    }
    if (!insideLoc)
        Serv.error_page.insert(std::make_pair(errorsnum, value));
}
