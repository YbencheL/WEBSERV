#include "ConfigPars.hpp"

void extracting_location_blocks(std::deque<Token>& tokenContainer , ServerBlock& Serv, ssize_t& i)
{
    bool InsideLocationBlock = false;
    ssize_t keepCountOfBrase = 0;
    ssize_t pos = 0;
    
    for (; i < (ssize_t)tokenContainer.size(); i++)
    {
        if (!InsideLocationBlock && (tokenContainer[i].value == "cgi_extension" || tokenContainer[i].value == "cgi_path"))
            error_line(": unkown keyword", tokenContainer[i].line);
        if (tokenContainer[i].value == "location")
        {
            LocationBlock loc;
            loc.autoindex = false;
            i++;
            InsideLocationBlock = true;
            while (InsideLocationBlock)
            {
                if (tokenContainer[i].value == "{")
                    keepCountOfBrase++;
                else if ((i + 1) < (ssize_t)tokenContainer.size() && (pos = tokenContainer[i].value.find_first_of("/")) != 0 && tokenContainer[i + 1].value == "{")
                    error_line(": paths must start with /", tokenContainer[i].line);
                else if ((i - 1) >= 0 && tokenContainer[i].type == 1 && tokenContainer[i - 1].value == "location")
                    loc.path = tokenContainer[i].value;
                else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "root")
                    loc.root = tokenContainer[i + 1].value;
                else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "index")
                {
                    i++;
                    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        loc.index.push_back(tokenContainer[i].value);
                        i++;
                    }
                }else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "allow_methods")
                {
                    i++;
                    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        if (tokenContainer[i].value == "GET" || tokenContainer[i].value == "POST" || tokenContainer[i].value == "DELETE")
                        {
                            loc.allow_methods.push_back(tokenContainer[i].value);
                            duplicate_check(loc.allow_methods, "GET");
                            duplicate_check(loc.allow_methods, "POST");
                            duplicate_check(loc.allow_methods, "DELETE");
                        }
                        else
                            error_line(": only allowed methods are (GET, POST, DELETE)", tokenContainer[i].line);
                        i++;
                    }
                }else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "autoindex")
                {
                    if (i < (ssize_t)tokenContainer.size() && tokenContainer[i + 1].value == "off")
                        loc.autoindex = false;
                    else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i + 1].value == "on")
                        loc.autoindex = true;
                    else
                        error_line(": auto index works with only on or off options", tokenContainer[i].line);
                }else if(i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "cgi_extension")
                {
                    i++;
                    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        loc.cgi_extension.push_back(tokenContainer[i].value);
                        i++;
                    }
                }else if (i < (ssize_t)tokenContainer.size() && tokenContainer[i].value == "cgi_path")
                {
                    i++;
                    while(i < (ssize_t)tokenContainer.size() && tokenContainer[i].type == 1)
                    {
                        loc.cgi_path.push_back(tokenContainer[i].value);
                        i++;
                    } 
                }
                else if (tokenContainer[i].value == "}")
                {
                    keepCountOfBrase--;
                    Serv.locations.push_back(loc);
                    InsideLocationBlock = false;
                    break;
                }else if (tokenContainer[i].value == "listen" || tokenContainer[i].value == "server_name" || tokenContainer[i].value == "error_page" ||
                    tokenContainer[i].value == "client_max_body_size")
                    error_line(": listen, server_name, client_mbs and error_pages must be inside server block not location", tokenContainer[i].line);
                i++;
            }
        }else if (tokenContainer[i].value == "{")
            keepCountOfBrase++;
        else if (tokenContainer[i].value == "}" && keepCountOfBrase)
        {
            keepCountOfBrase--;
            if (keepCountOfBrase == 0)
                break;
        }
    }           
}