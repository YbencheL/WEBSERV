#include "ConfigPars.hpp"

// looks for a serverblock in the config file
const ServerBlock* getServerForRequest(const std::string &ip, int port,
    const std::deque<ServerBlock> &serverConfigs
)
{
    if (ip.empty() || !port)
    {
        std::cerr << "you must provide an ip and port" << std::endl;
        return NULL;
    }
    for (size_t i = 0; i < serverConfigs.size(); ++i)
    {
        const ServerBlock &srv = serverConfigs[i];

        if (srv.listen != port)
            continue;
        if (srv.host != ip)
            continue;
        return (&srv);
    }
    return NULL;
}

// checks for exact path match in location in case it didnt find it it gives the default one if it exist
// this function uses prefix matche logic
const LocationBlock* getLocation(const std::string &path, const ServerBlock& srv)
{
    size_t bestmatch = 0;
    size_t matchedlength = 0;
    const LocationBlock *loc = NULL;

    for (size_t i = 0; i < srv.locations.size(); ++i)
    {
        if (srv.locations[i].path.size() > path.size())
            continue;
        matchedlength = 0;
        for (size_t j = 0; j < srv.locations[i].path.size(); j++)
        {
            if (path[j] == srv.locations[i].path[j])
                matchedlength++;
            else
                break;
        }
        if (matchedlength == srv.locations[i].path.size() && matchedlength > bestmatch)
        {
            bestmatch = matchedlength;
            loc = &srv.locations[i];
            if (srv.locations[i].path == path)
                return loc;
        }
    }
    return (loc);
}
