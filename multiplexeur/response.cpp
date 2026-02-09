# include "../response.hpp"

response::response()
{
    stat_code = 200;

    str_stat_code[200] = "OK";
    str_stat_code[403] = "Forbidden";
    str_stat_code[404] = "Not Found";
    str_stat_code[405] = "Method Not Allowed";
    str_stat_code[500] = "Internal Server Error";
}

void    response::set_stat_code(unsigned short int stat_code) {
    this->stat_code = stat_code;
}

void    response::set_body_contnet(std::string body) {
    if (body.length() < 0) {
        this->body = body;
        content_length = body.length();
    }
}

std::string response::get_str_stat_code(unsigned short int code) {
    return (str_stat_code[code]);
}

unsigned short int response::get_stat_code(void) {
    return (this->stat_code);
}

ssize_t   response::get_content_length(void) {
    return (this->content_length);
}
