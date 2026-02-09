# ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>

struct client;

class request {
    private:
        std::string method;
        std::string path;
        std::string query;
        std::string protocol;
        std::map<std::string, std::string> headers;
        unsigned int content_length;
        std::string body;
        
        // here i will know if there's WRONG REQUEST 'GETblabla'
        // about methode -> Error 405 (Method Not Allowed)
        // about protocol -> Error 400 (Bad Request)
        unsigned int request_stat;  
        
    public:
        request();

        std::string get_method() const;
        std::string get_path() const;
        std::string get_header(std::string key) const;
        // ...

        unsigned int get_req_stat() {
            return (request_stat);
        }

        // unsigned int get_request_stat() {
        //     return (request_stat);
        // }
};

# endif