#ifndef CONTENTVALIDATION_HPP
#define CONTENTVALIDATION_HPP

#include  <iostream>
#include  <deque>
#include  <vector>
#include  <cstdlib>
#include  <netdb.h>

//define
#define PORT_MIN_VAL 1024
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
    size_t line; 
};

// content check
void duplicate_check(std::deque<std::string>& keywords, std::string name);
void checking_for_virtual_hosts(std::deque<int>& seen);
int count_to_symbol(std::deque<Token>& tokenContainer, ssize_t& index, int count);

// syntax validation
void identifying_words_and_keywords(std::string& tok, std::deque<Token>& tokenContainer, int Line);
void identifying_symbols(std::string fileContent, size_t& i, std::string& tok, std::deque<Token>& tokenContainer,
    size_t& Line);
void is_syntax_valid(std::deque<Token> tokenContainer);
void comment_handle(std::string fileContent, size_t& i, size_t& Line);

// is syntax valid helpers
void keywords_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& LocationBlockCount, ssize_t& i, bool& insideServer);
void symbol_validation(std::deque<Token>& tokenContainer, ssize_t& ServerBlockCount, ssize_t& keepCountOfBrase, ssize_t& LocationBlockCount, ssize_t& i,
    bool& insideServer);

#endif