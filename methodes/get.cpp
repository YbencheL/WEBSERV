# include "../socket_engine.hpp"
# include <sys/stat.h>


/*  TODO: make the response
    1# >>> [] check if the path exist '../etc/passwod'
    2# >>> [] check if accessable too nahhhhhhhhhhhhhh yep fU*
    3# >>> [] we must use the root: path in the config file
    4# >>> [] apply status code in case of error, not found
    5# >>> [] Permission denied (HTTP 403 Forbidden)
    6# >>> [] check provided root and index in exist and prem
    7# >>> [] i have to get the index in the "locatoin /"
            config file (case of multiple source in index)
    8# >>> [] Default file(index.html) to serve when the 
            requested resource is a directory.
*/

std::string file_check() {

    struct stat statbuf;
    // std::string full_path;
    std::cout << "[>...................................] We get GET request [<]" << std::endl;

    if (path == "/")
        path = "/index.html";
    std::string full_path = root + path;

    
    // >>> redirections <<<
    if (stat(full_path.c_str(), &statbuf) < 0) {
        std::cout << "[LOG] The requested URL " << path << " was not found on this server." << std::endl;
        full_path = "www/not-found.html";
        status_code = 404;
    }
    else if (access(full_path.c_str(), F_OK | R_OK) < 0) {
        std::cout << "[LOG] The requested URL " << path << " is forbidden to access to." << std::endl;
        full_path = "www/forbidden.html";
        status_code = 403;
    }
    else if (S_ISDIR(statbuf.st_mode)) {
        // here i have to check if the autoindex is on, if not will throw an error not found page, right
        full_path += "/index.html";
        if (access(full_path.c_str(), F_OK | R_OK) < 0) {
            std::cout << "[LOG] The requested URL " << path << " is forbidden to access to." << std::endl;
            full_path = "www/forbidden.html";
            status_code = 403;
        }
    }
}