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
const LocationBlock* getLocation(const std::string &path, const ServerBlock& srv)
{
    const LocationBlock *Default;
    const LocationBlock *loc = NULL;

    for (size_t i = 0; i < srv.locations.size(); ++i)
    {
        if (srv.locations[i].path == "/")
            Default = &srv.locations[i];
        else if (srv.locations[i].path == path)
            loc = &srv.locations[i];
    }
    if (!loc && Default)
        return (Default);
    return (loc);
}
