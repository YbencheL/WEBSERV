#include "../includes/ConfigPars.hpp"
typedef void(*handler)(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
bool& insideLoc);

void handler_caller(std::map<std::string, handler>& handler_map)
{
    handler_map["listen"] = &handle_listen;
    handler_map["set_timeout"] = &handle_timeout;
    handler_map["host"] = &handle_host;
    handler_map["root"] = &handle_server_block_root;
    handler_map["client_max_body_size"] = &handle_server_block_client_mbs;
    handler_map["server_name"] = &handle_server_name;
    handler_map["error_page"] = &handle_error_page_server;
    handler_map["index"] = &handle_server_block_index;
}

void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i)
{
    int countARG = 0;
    std::map<std::string, handler> handler_map;

    handler_caller(handler_map);
    if (handler_map.find(tokenContainer[i].value) != handler_map.end())
        handler_map[tokenContainer[i].value](tokenContainer, Serv, countARG, i, insideLoc);
    else if (tokenContainer[i].value == "location")
        insideLoc = true;
    else if (insideLoc && (tokenContainer[i].value == "listen" || tokenContainer[i].value == "server_name" || tokenContainer[i].value == "error_page"))
            error_line(": listen, server_name, client_mbs and error_pages must be inside server block not location", tokenContainer[i].line);
    else if (!insideLoc && (tokenContainer[i].value == "autoindex" || tokenContainer[i].value == "return" || tokenContainer[i].value == "cgi_path" ||
            tokenContainer[i].value == "cgi_extension" || tokenContainer[i].value == "allow_methods"))
            error_line(": location only keyword inside server block", tokenContainer[i].line);
}

void extracting_server_blocks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs)
{
    ServerBlock Serv;
    ssize_t keepCountOfBrase = 0;
    bool insideLoc = false;

    // init
    Serv.client_max_body_size = 0;
    Serv.listen = 0; 
    Serv.set_timeout = 0;
    // duplicate check rule
    checking_for_keyword_dups(tokenContainer);
    // storing values
    for (ssize_t i = 0; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (tokenContainer[i].type == 0)
            extracting_values_from_server_block(tokenContainer, insideLoc, Serv, i);
        else if (tokenContainer[i].value == "{")
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