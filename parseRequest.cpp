#include "includes/parseRequest.hpp"
#include "includes/client.hpp"
#include "includes/request.hpp"

// new token
token *newToken(std::string data)
{
    token *newToken = new token;
    newToken->str   = data;
    newToken->next  = NULL;

    return newToken;
}

// free tokens
void freeTokens(token *tokens)
{
    token *tmp;

    while (tokens)
    {
        tmp = tokens->next;
        delete tokens;
        tokens = tmp;
    }
}

// inisialize client struct
void inisializeClient(Client &client)
{
    client.parse.body       = false;
    client.parse.reqLine    = false;
    client.req_ready        = false;
    client.parse.methods[0] = "GET";
    client.parse.methods[1] = "POST";
    client.parse.methods[2] = "DELETE";
}

// decide token range and return 1 in case of double space
int rangeToken(int begin, int &end, std::string &data)
{
    if (data[begin] == ' ')
        return 1;
    int i = begin + 1;
    for (; data[i]; i++)
    {
        if (data[i] == ' ')
            break;
    }
    if (i == begin)
        return 1;
    end = i;

    return 0;
}

// split data to tokens
token *splitDataToTokens(std::string data)
{
    token *tokens = NULL;
    token *tmp    = NULL;
    int    end    = 0;
    int    begin  = 0;

    for (int i = 0; i < 3; i++)
    {
        if (rangeToken(begin, end, data))
        {
            freeTokens(tokens);
            return NULL;
        }
        if (!tokens) // first token
        {
            tokens = newToken(data.substr(begin, end - begin));
            tmp    = tokens;
        }
        else
        {
            tmp->next = newToken(data.substr(begin, end - begin));
            tmp       = tmp->next;
        }
        if ((data[end] == '\0' || data[end + 1] == '\0') && i < 2)
        {
            freeTokens(tmp);
            return NULL;
        }
        begin = end + 1;
    }
    if (data[end] != '\0')
    {
        freeTokens(tmp);
        return NULL;
    }
    return tokens;
}

// check wich method
bool checkMethod(token *token, std::string *methods, int methodNumber)
{
    int i = 0;
    for (; i < methodNumber; i++)
    {
        if (token->str == methods[i])
            break;
    }
    if (i == methodNumber)
        return false;
    return true;
}

// set method and checks if there wll be a body or not
void setMethodBodyCheck(Client &client, token *tokens)
{
    client.req.setMethod(tokens->str);
    if (tokens->str == "POST")
        client.parse.body = true;
}

// parse path and check for query
bool checkSetPathQuery(Client &cleint, token *tokens)
{ // TODO : need to know if query should be flaged exist or not
    if (tokens->str[0] != '/')
        return false;
    if (tokens->str.find('\r') != std::string::npos ||
        tokens->str.find('\n') != std::string::npos)
        return false;
    if (tokens->str.find('?') != std::string::npos)
    {
        int queryStart = tokens->str.find('?');
        cleint.req.setPath(tokens->str.substr(0, queryStart));
        cleint.req.setQuery(tokens->str.substr(
            queryStart + 1, tokens->str.length() - queryStart
        ));
    }
    else
        cleint.req.setPath(tokens->str);
    return true;
}

// parse request line
int parseRequestLine(Client &client, std::string &data)
// in case of an error return the Error code thats will be handeled later
{
    std::cout << "data :" << data << std::endl; // debug
    token *tokens = splitDataToTokens(data);
    if (!tokens)
        return 400;
    if (!checkMethod(tokens, client.parse.methods, 3))
        return 405;
    setMethodBodyCheck(client, tokens);
    if (!checkSetPathQuery(client, tokens->next))
        return 400;

    //-------------------------------- test --------------------------------//
    token *tmp = tokens;
    while (tokens)
    {
        std::cout << "token :" << tokens->str << "." << std::endl;
        tokens = tokens->next;
    }
    tokens = tmp;
    freeTokens(tokens);
    //-------------------------------- test --------------------------------//
    return 0;
}

// request parser
int parseRequest(Client &client, std::string recivedData)
// TODO: return a specific Error code when somethig wrong
{
    // checks if request line is compeleted and parse it
    if (client.parse.reqLine == false)
    {
        client.parse.remaining.append(recivedData);
        std::cout << "we have :" << client.parse.remaining << std::endl;
        if (client.parse.remaining.find("\r\n") != std::string::npos)
        {
            std::string reqLine = client.parse.remaining.substr(
                0, client.parse.remaining.find("\r\n")
            );
            if (parseRequestLine(client, reqLine))
                // TODO : handle wich error code been sent
                return 400;
            client.parse.reqLine = true;
        }
        else
            return 0; // return until we have the full request line
    }
    return 200;
}