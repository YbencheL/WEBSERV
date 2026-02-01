#include "ConfigPars.hpp"

ServerBlock* getServerForRequest(const std::string &ip, int port, const std::string &host,
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

        if (srv.listen.find(port) == srv.listen.end())
            continue;
        if (srv.host.find(ip) == srv.host.end())
            continue;
        if (!host.empty())
        {
            if (srv.server_name.find(host) != srv.server_name.end())
                return const_cast<ServerBlock*>(&srv);
            else
                continue;
        }
        return const_cast<ServerBlock*>(&srv);
    }
    return NULL;
}
