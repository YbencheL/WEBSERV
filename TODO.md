TODO:
DONE - Redirects (return directive): Implemented, working.
DONE - Missing Content-Length on all responses
        Missing Content-Length on all responses: Some paths (e.g., POST 201) should include Content-Length: 0 (appears to be done now).
DONE - Magic numbers: TIMEOUT_LIMIT = 5, MAX_EVENTS = 64, buffer size 8192 — no explanation.
DONE - Some silent failures: If epoll operations fail, only std::cerr is written; program doesn't exit.
DONE - If epoll operations fail, only std::cerr is written; program doesn't exit.



    - No buffer pooling: Every client connection allocates new Client struct in map (fine for <1000 clients). is there a problem in case of having 20 worker with 5 times post 100MB
    - Test HEAD http://localhost:8080/
FATAL ERROR ON LAST TEST: Head "http://localhost:8080/": read tcp 127.0.0.1:44144->127.0.0.1:8080: read: connection reset by peer

TODO: FIX about pipe_to_cilent/pipe_write_to_clint leak



TODO CGI:
    - Fork failures: Cgi::execution() sets state = ERROR on fork failure, but no error log.
    - errno check is forbiden




TODO parsing:
    3xx (300..399) → must have a target path/URL
