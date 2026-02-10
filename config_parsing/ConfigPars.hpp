#ifndef ConfigPars_HPP
#define ConfigPars_HPP

#include  <iostream>
#include  <fstream>
#include  <deque>
#include <cctype>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <unistd.h>
#include <netdb.h>
#include <cstring>

//define
#define PORT_MIN_VAL 1025
#define PORT_MAX_VAL 65535
#define CLIENT_MAX_BODY_SIZE 1000000

enum TokenType {
    KEYWORD,
    WORD,
    SYMBOL
};

struct Token
{
    TokenType type;
    std::string value;
    int line; 
};

struct LocationBlock
{
    std::string path;
    std::string returN;
    int client_max_body_size;
    std::string root;
    std::deque<std::string> index;
    std::deque<std::string> allow_methods;
    bool autoindex;
    std::deque<std::string> cgi_extension;
    std::deque<std::string> cgi_path;
};

struct ServerBlock
{
    int listen;
    std::string root;
    std::string host;
    std::string server_name;
    int client_max_body_size;
    std::deque<std::string> index;
    std::map<std::deque<int>, std::string> error_page;
    std::deque<LocationBlock> locations;
};

// content checks
void error_line(std::string msg, int Line);
void duplicate_check(std::deque<std::string>& keywords, std::string name);
void checking_for_keyword_dups(std::deque<Token>& tokenContainer);
void checking_values(ServerBlock& Serv);
void checking_for_virtual_hosts(std::deque<int>& seen);
int count_to_symbol(std::deque<Token>& tokenContainer, ssize_t& index, int count);
// syntax validation
void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line);
void is_syntax_valid(std::deque<Token> tokenContainer);
// is syntax valid helpers
void keywords_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& LocationBlockCount, ssize_t& i, bool& insideServer);
void symbol_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& keepCountOfBrase, ssize_t& LocationBlockCount, ssize_t& i,
    bool& insideServer);
// block extraction parsing
void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i);
void extracting_server_blocks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs);
// server block helpers
void handle_listen(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i);
void handle_host(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i);
void handle_server_block_root(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_server_name(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i);
void handle_server_block_client_mbs(std::deque<Token>& tokenContainer, ServerBlock& Serv, int countARG, ssize_t& i,
    bool& insideLoc);
void handle_server_block_index(std::deque<Token>& tokenContainer, ServerBlock& Serv, ssize_t& i);
void handle_error_page(std::deque<Token>& tokenContainer, ServerBlock& Serv, ssize_t& i);
void extracting_location_blocks(std::deque<Token>& tokenContainer , ServerBlock& Serv, ssize_t& i);
// location block helpers
void handle_client_mbs(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i);
void handle_return(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i);
void handle_allow_methods(std::deque<Token>& tokenContainer, LocationBlock& loc, ssize_t& i);
void handle_index(std::deque<Token>& tokenContainer, LocationBlock& loc, ssize_t& i);
void handle_cgi(std::deque<Token>& tokenContainer, LocationBlock& loc, ssize_t& i);
void handle_autoindex(std::deque<Token>& tokenContainer, LocationBlock& loc, int countARG, ssize_t& i);
void extracting_blocks_plus_final_checks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& serverConfigs);
std::deque<ServerBlock> tokenzation(std::string fileContent);
//get_values
const ServerBlock* getServerForRequest(const int ip, int port, const std::deque<ServerBlock> &serverConfigs);
const LocationBlock* getLocation(const std::string &path, const ServerBlock& srv);
in_addr_t address_resolution(std::string host);

#endif