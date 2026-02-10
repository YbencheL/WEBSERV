# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <map>
# include <string>
# include <vector>
# include "request.hpp"
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
        bool    is_body_ready;
        
        // REQUEST HARDCODED VALUES TO TEST
        std::string method;
        std::string protocol;
        std::string path;
        std::string host;
        int port;
        // --------------------------------
        std::string resolved_path;

        std::map<std::string, std::string> header;
        std::string body;
    public:
        // INIT THAT SHIT
        response();
        response(const request& req);

        // MAIN FUNCTION
        void    response_handler(std::deque<ServerBlock> &server_config_info);
        void    static_files_handler();
        
        // SETTERS
        void    set_stat_code(unsigned short int stat_code);
        void    set_body_contnet(std::string body);
        void    set_resolved_path(std::string &re_path); // set
        void    set_body_as_ready(void);
        
        // GETTERS
        std::string         get_str_stat_code(unsigned short int code);
        unsigned short int  get_stat_code(void);
        ssize_t             get_content_length(void);
        std::string         get_path(void);
        bool                get_is_body_ready(void);
};

bool        is_allowd_method(std::deque<std::string> allow_methods, std::string method);
std::string index_file_iterator(std::string &full_path ,const std::deque<std::string> &index);
std::string html_index_gen(std::vector<std::string> &dir_list, std::string &uri_path);
void        autoindex_page(std::string &full_path, response &res);
std::string path_resolver(std::string root, std::string path);
void        path_validation(const LocationBlock *location_block, response &res);

# endif