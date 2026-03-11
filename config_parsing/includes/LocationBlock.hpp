#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include "ConfigPars.hpp"
#include <map>
#include <set>

struct ServerBlock;

struct LocationBlock
{
    std::string path;
    std::map<int, std::string> redirection;
    int client_max_body_size;
    std::string root;
    std::set<std::string> index;
    std::set<std::string> allow_methods;
    bool autoindex;
    std::map<std::string, std::string> cgi_handler;
    std::map<int, std::string> error_page;
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
void handle_location_block_root(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);
void extracting_blocks_plus_final_checks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& serverConfigs);
void handle_redirections(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i,
std::string& keyword);

//cgi path check
bool    is_cgi_path_valid(std::string interpreter_path);

#endif