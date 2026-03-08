# include "../response_builder.hpp"
# include "../utils/utils.hpp"

/*  TODO: "JUST the norm to follow" 
    keep the handle_post() small
    before implement any function ask me, it can be pre-made
    take a look of the "class response_builder" member to have a good understanding
*/

/*  >>> temp
    POST /upload HTTP/1.1
    Host: 10.11.10.5:8080
    Connection: keep-alive
    Content-Length: 196
    Cache-Control: max-age=0
    Origin: http://10.11.10.5:8080
    Content-Type: multipart/form-data; boundary=----WebKitFormBoundary2t2X4odHdADKlrta
    Upgrade-Insecure-Requests: 1
    User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/142.0.0.0 Safari/537.36
    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*//*;q=0.8,application/signed-exchange;v=b3;q=0.7
    Referer: http://10.11.10.5:8080/www
    Accept-Encoding: gzip, deflate
    Accept-Language: en-US,en;q=0.9,fr;q=0.8,ar;q=0.7

    ------WebKitFormBoundary2t2X4odHdADKlrta
    Content-Disposition: form-data; name="fileToUpload"; filename=""
    Content-Type: application/octet-stream


    ------WebKitFormBoundary2t2X4odHdADKlrta--

    ------- END REQUEST
*/

std::string file_name = "test.txt";
std::string extanssion = ".txt";
std::string file_content = "hello world it's just a test file :)";

/*  TODO-LIST
    - take the file name from the metadata of the body to create a local file in /upload
    - create that file name in local /upload folder and set the file content in the body
    - after the the response in case of seccussfully
        HTTP/1.1 201 Created
        Server: Webserv/1.0
        Date: Sat, 07 Mar 2026 13:30:00 GMT
        Content-Type: text/html
        Content-Length: 50

        <html><body><h1>File Uploaded Successfully!</h1></body></html>
    
    - what to do if
        empty filename with content?
        filename with empty content?
*/

void    response_builder::handle_post()
{

    // std::string full_path = this->current_client->location_conf->root + "/" + file_name;
    // // normalisePath(full_path, );
    
    // std::cout << "full_path >>> " << full_path << std::endl;

    // int o_file = open(file_name.c_str(), O_RDWR);
    // if (!o_file) {
    //     this->current_client->res.set_stat_code(SERVER_ERROR);
    //     return ;
    // }

    std::cout << "POST REQUEST LATER ON :(" << std::endl;
    exit(121);
}
