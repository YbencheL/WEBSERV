#ifndef PARSEREQUEST_HPP
#define PARSEREQUEST_HPP

#include <iostream>
#include "request.hpp"
#include "client.hpp"

struct reqParse
{
	std::string remaining;
    bool reqLine = false;
};

#endif