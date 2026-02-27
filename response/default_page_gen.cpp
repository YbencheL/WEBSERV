# include "../response_builder.hpp"
# include "../utils/utils.hpp"

# include "../socket_engine.hpp"    // use to use client struct

std::string   get_error_page_style()
{
    std::string error_page_style = "\n"
        "   * { padding: 0; margin: 0; }\n"
        "   body { display: flex; width: 100vw; height: 100vh; background-color: #ffffffff; }\n"
        "   .deful_page_container { display: flex; width: 100vw; height: 100vh; "
            "justify-content: center; align-items: center; flex-direction: column; "
            "font-family: \"Zalando Sans SemiExpanded\", sans-serif; }\n"
        "   .deful_page_container > * { padding-bottom: 20px; }\n"
        "   .deful_page_container h1 { font-size: 100px; }\n"
        "   .error_msg { display: flex; align-items: center; padding-bottom: 0; }\n"
        "   .go_back a { color: blue; }\n";
    return (error_page_style);
}

const std::string   get_autoindex_page_style()
{
    std::string autoindex_style = "\n"
        "   * { padding: 0; margin: 0; }\n"
        "   body { background-color: #ffffffff; padding: 40px; "
        "          font-family: \"Segoe UI\", \"Roboto\", Helvetica, Arial, sans-serif; }\n"
        "   .index_container { background: #ffffff; max-width: 800px; margin: 0 auto; "
        "                      padding: 20px; border-radius: 8px; "
        "                      box-shadow: 0 4px 10px rgba(0,0,0,0.1); }\n"
        "   h1 { border-bottom: 2px solid #eee; padding-bottom: 10px; "
        "        font-size: 24px; color: #555; }\n"
        "   a { display: block; padding: 8px 12px; text-decoration: none; "
        "       color: #0066cc; border-radius: 4px; transition: background 0.2s; }\n"
        "   a:hover { background-color: #e4e4e4ff; color: #003f8cff; }\n"
        "   .index_path { font-weight: bold; color: #ff0000; }\n"
        "   hr { border: 0; border-top: 1px solid #9a9a9a; margin: 20px 0; }\n";
    return (autoindex_style);
}

void    response_builder::default_error_page(unsigned short int stat_code)
{
    // std::string str_code = to_string(stat_code);
    const std::string status_code = to_string(stat_code);
    const std::string str_of_stat_code = stat_code_to_string(stat_code);
    const std::string title = "Webserv: " + status_code + str_of_stat_code;
    const std::string style = get_error_page_style();

    this->body = "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "\t<title>" + title + "</title>\n"
        "\t<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n"
        "\t<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n"
        "\t<link href=\"https://fonts.googleapis.com/css2?family=Zalando+Sans+SemiExpanded:ital,wght@0,200..900;1,200..900&display=swap\" rel=\"stylesheet\">\n"
        "\t<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=sentiment_very_dissatisfied\" />"
        "\t<style>" + style + "</style>\n"
        "</head>\n"
        "<body>\n"
        "\t<div class=\"deful_page_container\">\n"
        "\t\t<h1>" + status_code + "</h1>\n"
        "\t\t<p class=\"error_code\">" + str_of_stat_code + "</p>\n"
        "\t\t<p class=\"error_msg\"> The server was unable to complete your request. <span class=\"material-symbols-outlined\"> sentiment_very_dissatisfied </span> </p>\n"
        "\t\t<p class=\"go_back\"> <a href=\"/\"> Return to Home </a> </p>\n"
        "\t</div>\n"
        "</body>\n"
        "</html>";
    this->response_holder.append("Content-Length: " + to_string(body.size()) + "\r\n\r\n");
    this->response_holder.append(this->body);
    this->is_body_ready = true;
}

void    response_builder::autoindex_gen(std::vector<std::string> &dir_list, const std::string &uri_path)
{
    std::string style = get_autoindex_page_style();
    this->body = "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "\t<title> Index of " + uri_path + "</title>\n"
        "\t<style>" + style + "</style>\n"
        "</head>\n"
        "<body>\n"
        "\t<div class=\"index_container\">\n"
        "\t\t<a class=\"index_path\" href=\"../\">../</a><br>\n";
    // -------------------------------------------------------------- //

    for (size_t i = 0; i < dir_list.size(); i++) {
        std::string &name = dir_list.at(i);

        if (name == ".") continue;
        if (name == "..") continue;

        this->body.append("\t\t<a href=\"" + name + "\">" + name + "</a><br>\n");
    }
    this->body.append("\t\t<hr>\n\t</div>\n</body>\n</html>");
    this->is_body_ready = true;
}

void    response_builder::autoindex_page(const std::string &full_path)
{
    std::cout << "<<<<<<<<<<<<<< autoindex_page call >>>>>>>>>>>>>>" << std::endl; // rm-me
    std::cout << "PPPPPPPPPPPPPPPPP -> " << full_path << std::endl;
    DIR *dir = opendir(full_path.c_str());
    if (dir == NULL){
        exit(132);
        return (this->current_client->res.set_stat_code(FORBIDDEN_ACCESS), (void)0);
    }
    std::vector<std::string> dir_list;
    while (true)
    {
        dirent *read_dir = readdir(dir);
        if (read_dir == NULL)
            break;
        dir_list.push_back(read_dir->d_name);
    }
    autoindex_gen(dir_list, full_path);
    closedir(dir);
    
}
