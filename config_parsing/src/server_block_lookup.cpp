#include "../includes/ConfigPars.hpp"

ServerBlockLookup::ServerBlockLookup() : server(NULL) {}

ServerBlockLookup::ServerBlockLookup(const ServerBlockLookup& obj) {
    host = obj.host;
    port = obj.port;
    serverConfigs = obj.serverConfigs;
}

ServerBlockLookup& ServerBlockLookup::operator=(const ServerBlockLookup& obj) {
    if(this != &obj)
    {
        host = obj.host;
        port = obj.port;
        serverConfigs = obj.serverConfigs;
    }
    return *this;
}

// looks for a serverblock in the config file
void ServerBlockLookup::setServerForRequest(const int ip, const size_t port,
    const std::deque<ServerBlock> &serverConfigs
)
{
    int Hostip = 0;
    
    if (!ip || !port)
        server = NULL;
    server = NULL;
    for (size_t i = 0; i < serverConfigs.size(); ++i)
    {
        const ServerBlock &srv = serverConfigs[i];
        Hostip = address_resolution(srv.host);
        if (srv.listen != port)
            continue;
        if (Hostip != ip)
            continue;
        server = &srv;
    }
}

const LocationBlock* ServerBlockLookup::getLocation(const std::string &path)
{
    size_t bestmatch = 0;
    size_t matchedlength = 0;
    const LocationBlock *loc = NULL;

    if (server->locations.empty())
        return NULL;
    for (size_t i = 0; i < server->locations.size(); ++i)
    {
        if (server->locations[i].path.size() > path.size())
            continue;
        matchedlength = 0;
        for (size_t j = 0; j < server->locations[i].path.size(); j++)
        {
            if (path[j] == server->locations[i].path[j])
                matchedlength++;
            else
                break;
        }
        if (matchedlength == server->locations[i].path.size() && matchedlength > bestmatch)
        {
            bestmatch = matchedlength;
            loc = &server->locations[i];
            if (server->locations[i].path == path)
                return loc;
        }
    }
    return (loc);
}

ServerBlockLookup::ServerBlockLookup(size_t port, int host, std::deque<ServerBlock> &serverConfigs)
{
    setServerForRequest(host, port, serverConfigs);
}

const ServerBlock* ServerBlockLookup::getServer()
{
    return server;
}

ServerBlockLookup::~ServerBlockLookup() {}