#include "ConfigPars.hpp"
#include <vector>

in_addr_t address_resolution(std::string host)
{
    struct addrinfo hints, *result;
    in_addr_t host_re;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), NULL, &hints, &result) != 0)
        return (INADDR_NONE);

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    
    host_re = ipv4->sin_addr.s_addr;

    freeaddrinfo(result);
    return (host_re);
}

// looks for a serverblock in the config file
const ServerBlock* getServerForRequest(const int ip, int port,
    const std::deque<ServerBlock> &serverConfigs
)
{
    int Hostip = 0;
    
    if (!ip || !port)
    {
        std::cerr << "you must provide an ip and port" << std::endl;
        return NULL;
    }
    for (size_t i = 0; i < serverConfigs.size(); ++i)
    {
        const ServerBlock &srv = serverConfigs[i];
        Hostip = address_resolution(srv.host);
        if (srv.listen != port)
            continue;
        if (Hostip != ip)
            continue;
        return (&srv);
    }
    return NULL;
}

// std::string path_normalize(const std::string root, std::vector<std::string> path_holder)
// {
//     (void)root;
//     std::string final_url;

//     for (size_t i = 0; i < path_holder.size(); i++) {
//         final_url.append("/");
//         final_url.append(path_holder.at(i));
//     }
//     std::cout << ">>> request path afterrrrrrrrrrrrrrrrrr >>> " << final_url << std::endl;
//     return (final_url);
// }

// std::string   path_resolver(std::string request_path)
// {
//     std::string root;
//     std::string path;
//     std::vector<std::string> path_holder;
//     size_t      start = 0;
//     size_t      end;

//     while ((end = request_path.find("/", start)) != std::string::npos)
//     {
//         std::string segment = request_path.substr(start, (end - start));
//         if (segment == ".." && path_holder.empty()) {
//             start = end + 1;
//             continue;
//         }
//         else if (segment == "..")
//             path_holder.pop_back();
//         else if (!segment.empty())
//             path_holder.push_back(segment);
//         start = end + 1;
//     }
//     path_holder.push_back(request_path.substr(start));
//     path = path_normalize(root, path_holder);
    
//     return path;
// }

const LocationBlock* getLocation(const std::string &path, const ServerBlock& srv)
{
    size_t bestmatch = 0;
    size_t matchedlength = 0;
    const LocationBlock *loc = NULL;
    // std::string newPath;

    // newPath = path_resolver(path);
    // if(newPath.empty())
    //     return NULL;
    // std::cout << "path : " << newPath << std::endl;
    if (srv.locations.empty())
        return NULL;
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
