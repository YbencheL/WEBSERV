# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <map>
# include <string>
# include "config_parsing/ConfigPars.hpp"

# define OK 200
# define FORBIDDEN_ACCESS 403
# define NOT_FOUND 404
# define METHOD_NOT_ALLOWED 405
# define SERVER_ERROR 500

struct client;

// JUST A TEMPLATE NOT USED DIRRECT
class response
{
    private:
        unsigned short int stat_code;
        ssize_t content_length;
        std::map<unsigned short int, std::string> str_stat_code;
        std::string path;
        std::map<std::string, std::string> header;
        std::string body;
    public:
        response();
        
        // SETTERS
        void set_stat_code(unsigned short int stat_code);
        void set_body_contnet(std::string body);
        
        // GETTERS
        std::string get_str_stat_code(unsigned short int code);
        unsigned short int  get_stat_code(void);
        ssize_t   get_content_length(void);
};

void        response_handler(std::deque<ServerBlock> &server_config_info, client &current_client);
bool        is_allowd_method(std::deque<std::string> allow_methods, std::string method);
void        path_validation(const LocationBlock *location_block, client &client);
std::string index_file_iterator(std::string &full_path ,const std::deque<std::string> &index);
void        autoindex_page(std::string &full_path, client &current_client);
std::string path_resolver(std::string root, std::string path);


# endif