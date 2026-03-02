# ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <sstream>
# include <vector>

const std::string   to_string(int digit);
const std::string   file_to_string(const std::string& path);
const std::string   &stat_code_to_string(unsigned short int stat_code);
const std::string   resolved_path_extension(std::string path);
std::string         get_time();
bool                is_valid_error_path(std::string path);
std::string path_normalize(std::vector<std::string> path_holder);
std::string   path_resolver(std::string request_path);
std::vector<std::string>    host_splite(std::string Host);
unsigned short int  valid_port_number(std::string port_num);

# endif