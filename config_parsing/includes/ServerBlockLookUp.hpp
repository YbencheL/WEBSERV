#ifndef SERVERBLOCKLOOKUP_HPP
#define SERVERBLOCKLOOKUP_HPP

#include "ConfigPars.hpp"
#include "ServerBlock.hpp"

class ServerBlockLookup {
    private:
        size_t port;
        int host;
        std::deque<ServerBlock> serverConfigs;
        const ServerBlock* server;
    public:
        ServerBlockLookup();
        void setServerForRequest(const int ip, const size_t port,
            const std::deque<ServerBlock> &serverConfigs);
        ServerBlockLookup(size_t port, int host, std::deque<ServerBlock> &serverConfigs);
        ServerBlockLookup(const ServerBlockLookup& obj);
        ServerBlockLookup& operator=(const ServerBlockLookup& obj);
        const ServerBlock* getServer();
        const LocationBlock* getLocation(const std::string &path);
        ~ServerBlockLookup();
};

#endif