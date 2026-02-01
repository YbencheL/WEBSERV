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

        if (srv.listen == port)
            continue;
        if (srv.host == ip)
            continue;
        if (!host.empty())
        {
            if (srv.server_name != host)
                return const_cast<ServerBlock*>(&srv);
            else
                continue;
        }
        return const_cast<ServerBlock*>(&srv);
    }
    return NULL;
}
