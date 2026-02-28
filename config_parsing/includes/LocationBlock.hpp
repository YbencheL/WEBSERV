#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include "ConfigPars.hpp"
#include <map>

struct ServerBlock;

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


#endif