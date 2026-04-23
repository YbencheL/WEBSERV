#ifndef ConfigPars_HPP
#define ConfigPars_HPP

#include "ContentValidation.hpp"
#include "ServerBlockLookUp.hpp"
#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include  <iostream>
#include  <fstream>
#include  <deque>
#include  <cctype>
#include  <sstream>
#include  <map>
#include  <set>
#include  <algorithm>
#include  <cstring>
#include  <unistd.h>

// content checks
void checking_values(ServerBlock& Serv);

// block extraction parsing
void extracting_values_from_server_block(std::deque<Token>& tokenContainer, bool& insideLoc, ServerBlock& Serv, ssize_t& i);
void extracting_server_blocks(std::deque<Token>& tokenContainer, std::deque<ServerBlock>& ServerConfigs);

// main function
std::deque<ServerBlock> tokenzation(std::string fileContent);

// debugging
// void debugging(std::deque<ServerBlock>& serverConfigs);

//helper functions
void duplicate_check(std::deque<std::string>& keywords, std::string name);
bool    is_cgi_path_valid(std::string interpreter_path);
in_addr_t address_resolution(std::string host);
void error_line(std::string msg, int Line);

//get_values
// const ServerBlock* getServerForRequest(const int ip, const size_t port, const std::deque<ServerBlock> &serverConfigs);
// const LocationBlock* getLocation(const std::string &path, const ServerBlock& srv);

#endif