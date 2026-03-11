#ifndef CONTENTVALIDATION_HPP
#define CONTENTVALIDATION_HPP

#include  <iostream>
#include  <deque>
#include  <cstdlib>
#include  <netdb.h>

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

// content check
void error_line(std::string msg, int Line);
void duplicate_check(std::deque<std::string>& keywords, std::string name);
void checking_for_keyword_dups(std::deque<Token>& tokenContainer);
void checking_for_virtual_hosts(std::deque<int>& seen);
int count_to_symbol(std::deque<Token>& tokenContainer, ssize_t& index, int count);

// syntax validation
void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line);
void is_syntax_valid(std::deque<Token> tokenContainer);

// is syntax valid helpers
void keywords_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& LocationBlockCount, ssize_t& i, bool& insideServer);
void symbol_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& keepCountOfBrase, ssize_t& LocationBlockCount, ssize_t& i,
    bool& insideServer);

// address check
in_addr_t address_resolution(std::string host);

#endif