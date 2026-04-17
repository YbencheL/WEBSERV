# WEBSERV

*This project has been created as part of the 42 curriculum by [abenzaho], [yanflous], and [ybenchel].*

## Description

This project focuses on building our own HTTP web server from scratch. The goal is to understand how web servers work internally by implementing their core components and handling real HTTP communication.

To better understand the scope of this project, it is important to first define what an HTTP web server is.

HTTP (HyperText Transfer Protocol) is a protocol built on top of TCP/IP. It allows communication between two machines through a request/response model.

HTTP is a stateless protocol, meaning that each request is independent and the connection does not retain information between requests. This is why mechanisms like *cookies* are used—to simulate stateful behavior on top of a stateless protocol.

In HTTP, client interactions are performed using methods such as GET, POST, DELETE, etc. Each request sent by the client is processed by the server, which then returns a response.

As for HTTP response it is mainly composed of two parts:
- A status code (indicating the result of the request)
- A body (the data returned to the client)

## Instructions

To run the web server, first compile the project using `make`. This will build the executable `webserv`, which requires a configuration file as an argument.

You can find example configuration files inside the `config_files` directory.

### Available Make commands

- `make`  
  Compiles all source files and generates the `webserv` executable.

- `make clean`  
  Removes all object files.

- `make fclean`  
  Removes all object files as well as the executable.

- `make re`  
  Recompiles the entire project (equivalent to running `make fclean` followed by `make`).

### Running the server

run this on the terminal: ./webserv <configuration_file>

### Testing for leaks

run this on the terminal: valgrind --track-origins=yes ./webserv <configuration_file>

## Resources

For this project, we relied on a combination of personal research and well-known documentation.

One of our teammates, yanflous, created a helpful resource that you can find here:  
<https://medium.com/@ma1loc/web-d78223b4573a>

Since this project revolves around HTTP and web servers, there are many high-quality references available. Here are some of the most useful ones:

- Nginx documentation (configuration and behavior):  <https://nginx.org/en/docs/>

### AI Usage in the Project

AI tools were mainly used as a sort of an interactive documentation platfrom. They helped us better understand specific concepts, such as how certain components work and how they behave in different scenarios.

Additionally, AI was used for:
- Providing ideas for testing different parts of the project
- Assisting with grammar and wording improvements in this README

AI used as a support tool to enhance understanding and improve documentation quality.
