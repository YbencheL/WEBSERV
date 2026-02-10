#include "includes/parseRequest.hpp"

int    parseRequestLine(client &client,std::string &data)
//in case of an error return the Error code thats will be handeled later
{

}

int parseRequest(client &client, std::string recivedData)
//TODO: return a specific Error code when somethig wrong
{
	//checks if request line is compeleted and parse it
    if (client.parse.reqLine == false)
    {
        client.parse.remaining.append(recivedData);
        if (client.parse.remaining.find("\r\n"))
        {
            client.parse.reqLine = true;
            std::string reqLine = recivedData.substr(0, recivedData.find("\r\n"));
            parseRequestLine(client, reqLine);
        }
    }

    return 200;
}