#include "../includes/ConfigPars.hpp"

typedef void(*handler)(std::deque<Token>&, LocationBlock&, int, ssize_t&, std::string&);

void handler_caller(std::map<std::string, handler>& handler_map)
{
    handler_map["client_max_body_size"] = &handle_client_mbs;
    handler_map["return"] = &handle_redirections;
    handler_map["index"] = &handle_index;
    handler_map["error_page"] = &handle_redirections;
    handler_map["allow_methods"] = &handle_allow_methods;
    handler_map["autoindex"] = &handle_autoindex;
    handler_map["cgi_extension"] = &handle_cgi;
    handler_map["cgi_path"] = &handle_cgi;
}

bool brackets_count(std::string value, ssize_t& keepCountOfBrase, bool& InsideLocationBlock, std::deque<LocationBlock>& locations,
LocationBlock* loc)
{
    if (value == "{")
        keepCountOfBrase++;
    else if (value == "}" && InsideLocationBlock)
    {
        keepCountOfBrase--;
        locations.push_back(*loc);
        InsideLocationBlock = false;
        return false;
    }
    else if (!InsideLocationBlock && (value == "}" && keepCountOfBrase))
    {
        keepCountOfBrase--;
        if (keepCountOfBrase == 0)
            return false;
    }else if (InsideLocationBlock && (value == "listen" || value == "server_name" ||
            value == "host" || value == "set_timeout"))
    {
        std::cout << value << std::endl;
        error_line(": server only keyword inside location block", -1);
    }
    return true;
}

void extracting_location_blocks(std::deque<Token>& tokenContainer , ServerBlock& Serv, ssize_t& i)
{
    bool InsideLocationBlock = false;
    ssize_t keepCountOfBrase = 0;
    int countARG = 0;
    std::map<std::string, void(*)(std::deque<Token>&, LocationBlock&,
        int, ssize_t&, std::string&)> handler_map;
    ssize_t pos = 0;

    handler_caller(handler_map);
    for (; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (tokenContainer[i].value == "location")
        {
            LocationBlock loc;
            loc.client_max_body_size = 0;
            loc.autoindex = false;
            i++;
            InsideLocationBlock = true;
            while (InsideLocationBlock)
            {
                if (!brackets_count(tokenContainer[i].value, keepCountOfBrase, InsideLocationBlock, Serv.locations, &loc))
                    break;
                else if ((i + 1) < (ssize_t)tokenContainer.size() && (pos = tokenContainer[i].value.find_first_of("/")) != 0
                    && tokenContainer[i + 1].value == "{")
                    error_line(": paths must start with /", tokenContainer[i].line);
                else if ((i - 1) >= 0 && tokenContainer[i].type == 1 && tokenContainer[i - 1].value == "location")
                    loc.path = tokenContainer[i].value;
                else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "root")
                    loc.root = tokenContainer[i + 1].value;
                else if (handler_map.find(tokenContainer[i].value) != handler_map.end())
                    handler_map[tokenContainer[i].value](tokenContainer, loc, countARG, i, tokenContainer[i].value);
                i++;
            }
        }else if (!brackets_count(tokenContainer[i].value, keepCountOfBrase, InsideLocationBlock, Serv.locations, NULL))
            break;
    }           
}