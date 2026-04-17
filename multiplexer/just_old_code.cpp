else if (pipe_to_client.count(fd))  // reading -> pipeOut[0]
			{
				int client_socket = pipe_to_client[fd];
				this->raw_client_data[client_socket].last_activity = time(0);
				this->raw_client_data[client_socket].cgiHandler.reading();
				
				cgiState stat = this->raw_client_data[client_socket].cgiHandler.state;
				if (stat == CGI_DONE || stat == ERROR)
                {
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					remove_fd_from_list(fd);
					pipe_to_client.erase(fd);

					raw_client_data[client_socket].last_activity = time(0);

					if (stat == CGI_DONE) {
						response_builder rb;
						rb.set_client(raw_client_data[client_socket]);
						rb.build_cgi_response(raw_client_data[client_socket].cgiHandler.getResponse());
					}
                    else
                    {
						raw_client_data[client_socket].res.set_stat_code(SERVER_ERROR);
						response_builder rb;
						rb.set_client(raw_client_data[client_socket]);
						rb.build_response();
					}

                    // {   // after EPOLLOUT ->  is ready for writing
                    struct epoll_event ev;
                    ev.events = EPOLLOUT;
                    ev.data.fd = client_socket;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev);
                    // }
				}
            }
			else if (pipe_write_to_client.count(fd)) // writing -> pipeIn[1]
			{

				int client_socket = pipe_write_to_client[fd];
				Client &c = raw_client_data[client_socket];
				const std::string &body = c.req.getBody();
				off_t &sent = c.cgiHandler.body_bytes_sent;
				
				// rm-me
				std::cerr << "PIPE_WRITE FIRED sent=" << c.cgiHandler.body_bytes_sent << "/" << body.size() << std::endl;
				
				ssize_t n = write(fd, body.c_str() + sent, body.size() - sent);

                // rm-me
				std::cerr << "write returned: " << n << " errno: " << errno << " remaining: " << (body.size() - sent) << std::endl;
				if (n > 0) {
					sent += n;
					raw_client_data[client_socket].last_activity = time(0);
				}

				if (sent >= (off_t)body.size())
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					remove_fd_from_list(fd);
					pipe_write_to_client.erase(fd);
					close(fd);
				}
			}

// Client request → Remove client FD from epoll → CGI starts → Wait for CGI_DONE → Re-add client FD
// Client request → Remove client FD → Add pipe FDs → Read CGI chunks → Send to client → When EOF, re-add client FD