# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <map>
# include <string>
# include <vector>
# include "config_parsing/includes/ConfigPars.hpp"
# include "./cookies_sessions/includes/SessionManager.hpp"

# define OK 200
# define CREATED 201
# define NO_CONTENT 204
# define MOVED_PERMANENTLY 301
# define FOUND 302
# define BAD_REQUEST 400
# define FORBIDDEN_ACCESS 403
# define NOT_FOUND 404
# define METHOD_NOT_ALLOWED 405
# define REQUEST_TIMEOUT 408
# define PAYLOAD_TOO_LARGE 413
# define URI_TOO_LONG 414
# define HEADER_TOO_LARGE 431
# define SERVER_ERROR 500
# define VERSION_NOT_SUPP 505

# define REQ_NOT_READY 0
# define PROTOCOL_VERSION   "HTTP/1.0"

struct Client;

class response
{
    private:
        int                 port;
        std::string         host;
        std::string         final_raw_response;

        int             static_file_fd;
        off_t           file_size;
        off_t           bytes_sent;

        std::string         path;
        unsigned short int  stat_code;
        bool                is_body_ready;
        std::string         content_type;
        ssize_t             content_length;

        bool                        is_cooke_set;
        std::vector<std::string>    cookie_holder;

    public:
        response();

        // >> setters
        void    set_static_file_fd(int fd);
        void    set_stat_code(unsigned short int stat_code);
        void    set_path(std::string path);
        void    set_raw_response(std::string &raw_res);
        void    set_file_size(off_t file_size);
        void    save_bytes_sent(off_t bytes_sent);
        bool    stream_response_to_client(int fd);


        // >> getters
        std::string         get_str_stat_code(unsigned short int code) const;
        unsigned short int  get_stat_code(void) const;
        ssize_t             get_content_length(void) const;
        std::string         get_path(void) const;
        std::string         get_start_line(void) const;
        std::string         &get_raw_response(void);
        int                 get_static_file_fd(void) const;
        off_t               get_file_size(void) const;
        off_t               get_bytes_sent(void) const;

        // >> cookie
        void                handle_session(SessionManager &session_manager, Client &client);
        const               std::vector<std::string> &get_cookie_holder() const;
        bool                get_is_cookie_set() const;
        const               std::vector<std::string>& get_set_cookie_headers() const;
        void                set_is_cookie_false();
};

# endif
