# ifndef RESPONSE_BUILDER_HPP
# define RESPONSE_BUILDER_HPP

# include <deque>
# include <vector>
# include <dirent.h>
# include <sys/stat.h>
# include "response.hpp"
# include "./config_parsing/ConfigPars.hpp"
# include "client.hpp"


# define GET_METHODE "GET"
# define POST_METHODE "POST"
# define DELETE_METHODE "DELETE"

struct client;

class response_builder
{
    private:
        Client                  *current_client;
        // const ServerBlock       *server_conf;
        // const LocationBlock     *location_conf;
        std::string             response_holder;
        std::string             path;
        std::string             header;
        std::string             body;
        bool                    is_body_ready;
        bool                    is_error_page;
        
        
        bool            is_allowd_method(std::string method);
        void            path_validation(void);
        std::string     index_file_iterator(const std::string &full_path);
        void            autoindex_gen(std::vector<std::string> &dir_list, const std::string &uri_path);
        void            autoindex_page(const std::string &full_path);
        void            default_error_page(unsigned short int stat_code);
        std::string     get_stat_code_path(unsigned int stat_code);
        void            generate_error_page();
        void            set_header(void);
        void            set_body(void);
        void            handle_get();
        void            handle_post();
        void            handle_delete();

        void            response_setup(void);   // MAKE "response_holder"
    public:
        response_builder();
        // void    init_response_builder(Client &current_client, std::deque<ServerBlock> &config);
        void    init_response_builder(Client &current_client);
        void    validate_headers(std::map<std::string, std::string> header);
        void    build_response();
};

# endif
