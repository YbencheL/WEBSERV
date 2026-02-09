#include "ConfigPars.hpp"

void debugging(std::deque<ServerBlock>& serverConfigs)
{
    for (size_t i = 0; i < serverConfigs.size(); i++)
    {
        std::cout << "===== ServerBlock #" << i << " =====" << std::endl;
        std::cout << "listen: " << serverConfigs[i].listen << std::endl;
        std::cout << "root: " << serverConfigs[i].root << std::endl;
        std::cout << "host: " << serverConfigs[i].host << std::endl;
        std::cout << "server_name: " << serverConfigs[i].server_name << std::endl;
        std::cout << "client_max_body_size: " << serverConfigs[i].client_max_body_size << std::endl;
        
        std::cout << "--- error_page ---" << std::endl;
        for (std::map<std::deque<int>, std::string>::iterator it = serverConfigs[i].error_page.begin();
            it != serverConfigs[i].error_page.end(); ++it)
        {
            const std::deque<int>& codes = it->first;
            const std::string& page = it->second;

            for (std::deque<int>::const_iterator c = codes.begin();
                c != codes.end(); ++c)
            {
                std::cout << *c << " -> " << page << std::endl;
            }
        }

        std::cout << "--- index ---" << std::endl;
        for (std::deque<std::string>::iterator it = serverConfigs[i].index.begin();
            it != serverConfigs[i].index.end(); ++it)
        {
            std::cout << *it << std::endl;
        }

        std::cout << "--- locations ---" << std::endl;
        for (size_t j = 0; j < serverConfigs[i].locations.size(); j++)
        {
            LocationBlock &loc = serverConfigs[i].locations[j];
            std::cout << "Location #" << j << ":" << std::endl;
            std::cout << "  path: " << loc.path << std::endl;
            std::cout << "  root: " << loc.root << std::endl;
            std::cout << "  return: " << loc.returN << std::endl;
            std::cout << "  client_max_body_size: " << loc.client_max_body_size << std::endl;

            std::cout << "  index: ";
            for (size_t k = 0; k < loc.index.size(); k++)
                std::cout << loc.index[k] << " ";
            std::cout << std::endl;

            std::cout << "  allow_methods: ";
            for (size_t k = 0; k < loc.allow_methods.size(); k++)
                std::cout << loc.allow_methods[k] << " ";
            std::cout << std::endl;

            std::cout << "  autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;

            std::cout << "  cgi_path: ";
            for (size_t k = 0; k < loc.cgi_path.size(); k++)
                std::cout << loc.cgi_path[k] << " ";
            std::cout << std::endl;

            std::cout << "  cgi_extension: ";
            for (size_t k = 0; k < loc.cgi_extension.size(); k++)
                std::cout << loc.cgi_extension[k] << " ";
            std::cout << std::endl;

            std::cout << "------------------" << std::endl;
        }
        std::cout << "==============================" << std::endl << std::endl;
    }
    //these function return NULL when fail !!!!
    const ServerBlock* server = getServerForRequest("127.0.0.1", 8080, serverConfigs);
    if (server)
        std::cout << server->listen << std::endl;
    if (server)
    {
        const LocationBlock* location = getLocation("/cgi-bi", *server);
        if (location)
            std::cout << location->path << std::endl;
    }
}

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cout << "Error: file is missing" << std::endl;
        return 1;
    }
    std::ifstream infile(av[1]);
    std::string fileContent((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    if (infile.fail())
    {
        std::cout << "Error: can't open file!" << std::endl;
        return 1;
    }
    try
    {
        tokenzation(fileContent);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}