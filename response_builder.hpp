# ifndef RESPONSE_BUILDER_HPP
# define RESPONSE_BUILDER_HPP

# include <deque>
# include <vector>
# include <dirent.h>
# include <sys/stat.h>
# include "response.hpp"
# include "./config_parsing/includes/ConfigPars.hpp"
# include "./socket_engine.hpp"
# include "client.hpp"


# define GET_METHODE "GET"
# define POST_METHODE "POST"
# define DELETE_METHODE "DELETE"

struct client;
extern socket_engine s_engine;

class response_builder
{
    private:
        Client                  *current_client;
        std::string             response_holder;
        std::string             path;

        std::string             header_buff;
        std::string             body_buff;
        
        bool                    is_body_ready;
        bool                    is_error_page;
        
        bool            is_allowd_method(std::string method);
        void            path_validation(void);
        std::string     index_file_iterator(const std::string &full_path);
        void            autoindex_gen(std::vector<std::string> &dir_list, const std::string &full_path, const std::string &request_uri);
        void            autoindex_page(const std::string &full_path, const std::string &request_uri);
        void            default_error_page(unsigned short int stat_code);
        std::string     get_stat_code_path(unsigned int stat_code);
        void            generate_error_page();
        void            extract_host_info(std::string raw_req);

        // handling the rediction with the return
        void            return_handling();

        void            set_header(void);
        void            set_body(void);
        void            handle_get();
        void            handle_post();
        void            handle_delete();
        void            serving_static_file();
        
    public:
        response_builder();
        void    init_response_builder(Client &current_client);
        void    validate_headers();
        void    build_response();

};

bool    validate_headers(Client &current_client);

# endif
