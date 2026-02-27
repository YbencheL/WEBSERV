# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <map>
# include <string>
# include <vector>
# include "config_parsing/ConfigPars.hpp"

# define OK 200
# define BAT_REQUEST 400
# define FORBIDDEN_ACCESS 403
# define NOT_FOUND 404
# define METHOD_NOT_ALLOWED 405
# define PAYLOAD_TOO_LARGE 413
# define SERVER_ERROR 500
# define METHOD_NOT_IMPLEMENTED 501
# define VERSION_NOT_SUPP 505

# define REQ_NOT_READY 0
# define PROTOCOL_VERSION   "HTTP/1.0"

// PATH REQUEST TEST
// # define PATH0  "/www/index.html"                                        // PASS
# define PATH0  "/www/../"                                        // PASS
// # define PATH0  "/www/../www"                                            // PASS
// # define PATH0  "/www/../www/index.html"                                 // PASS
// # define PATH0  "/www/../www/secret.html"                                // PASS
// # define PATH0  "/www/../../../../../yanflous/Documents/index.html"      // PASS
// ------------------------------------------------------
// request normalization test
// # define PATH0  "/./www/index.html"
// # define PATH0  "/./www//index.html"
// # define PATH0  "//www//index.html"
// # define PATH0  "./www/index.html"
// ------------------------------------------------------
// # define PATH0  "/www"                                                   // PASS
// # define PATH0  "/www/"                                                  // PASS
// # define PATH0   "www/secret.html"                                       // PASS
// # define PATH0   "www/my%20file.html"                                    // NOT-YET
// ------------------------------------------------------


class response  // DONE[]
{
    private:
        int         port;
        std::string host;
        std::string         raw_response;   //  new

        std::string         path;
        unsigned short int  stat_code;
        bool                is_body_ready;
        
        //  USED IN THE HEADER RESPONSE
        std::string         content_type;
        ssize_t             content_length;

    public:
        response();

        // SETTERS
        void    set_stat_code(unsigned short int stat_code);
        void    set_path(std::string path);
        void    set_raw_response(std::string raw_res);

        // GETTERS
        std::string         get_str_stat_code(unsigned short int code) const;
        unsigned short int  get_stat_code(void) const;
        ssize_t             get_content_length(void) const;
        std::string         get_path(void) const;
        std::string         get_start_line(void) const;
        std::string         get_raw_response(void);

};

# endif
