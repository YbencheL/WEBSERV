# include "../response.hpp"
# include "../utils/utils.hpp"

// # include <ctime>

response::response() {
    this->stat_code = 200;
    this->content_length = 0;
    this->is_body_ready = false;
    this->path = "";

};

void    response::set_stat_code(unsigned short int stat_code) {
    this->stat_code = stat_code;
}

void    response::set_path(std::string path) {
    this->path = path;
}

void    response::set_raw_response(std::string raw_res) {
    this->raw_response = raw_res;
}

unsigned short int response::get_stat_code(void) const {
    return (this->stat_code);
}

ssize_t   response::get_content_length(void) const {
    return (this->content_length);
}

std::string response::get_path(void) const {
    return (this->path);
}

std::string response::get_raw_response(void) {
    return (this->raw_response);
}


std::string response::get_start_line()const 
{
    std::string start_line_gen;

    start_line_gen.append(PROTOCOL_VERSION);
    start_line_gen.append(" ");
    start_line_gen.append(to_string(stat_code) + " ");
    start_line_gen.append(stat_code_to_string(stat_code) + "\r\n");

    return (start_line_gen);
}
