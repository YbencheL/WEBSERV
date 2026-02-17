#ifndef ConfigPars_HPP
#define ConfigPars_HPP

#include "ContentValidation.hpp"
#include  <iostream>
#include  <fstream>
#include  <deque>
#include  <cctype>
#include  <sstream>
#include  <map>
#include  <set>
#include  <algorithm>
#include  <cstring>

struct LocationBlock
{
    std::string path;
    std::map<std::deque<int>, std::string> redirection;
    int client_max_body_size;
    std::string root;
    std::deque<std::string> index;
    std::deque<std::string> allow_methods;
    bool autoindex;
    std::deque<std::string> cgi_extension;
    std::deque<std::string> cgi_path;
    std::map<std::deque<int>, std::string> error_page;
};

struct ServerBlock
{
    int listen;
    int set_timeout;
    std::string root;
    std::string host;
    std::string server_name;
    int client_max_body_size;
    std::deque<std::string> index;
    std::map<std::deque<int>, std::string> error_page;
    std::deque<LocationBlock> locations;
};

// content checks
void checking_values(ServerBlock& Serv);

// block extraction parsing
void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i);
void extracting_server_blocks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs);

// server block helpers
void handle_listen(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_host(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_server_block_root(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_server_name(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_server_block_client_mbs(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_timeout(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_server_block_index(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_error_page_server(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void extracting_location_blocks(std::deque<Token>& tokenContainer , ServerBlock& Serv, ssize_t& i);

// location block helpers
void handle_client_mbs(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);
void handle_allow_methods(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);
void handle_index(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);
void handle_cgi(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);
void handle_autoindex(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);
void extracting_blocks_plus_final_checks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& serverConfigs);
void handle_redirections(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);

// main function
std::deque<ServerBlock> tokenzation(std::string fileContent);

// debugging
void debugging(std::deque<ServerBlock>& serverConfigs);

//get_values
const ServerBlock* getServerForRequest(const int ip, int port, const std::deque<ServerBlock> &serverConfigs);
const LocationBlock* getLocation(const std::string &path, const ServerBlock& srv);

#endif