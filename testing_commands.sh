curl -X POST http://10.11.10.6:8080/ -H "Content-Length: 0" -v
curl -v -X HEAD http://10.11.10.6:8080//
curl -v http://10.11.10.6:8080//directory
curl -v http://10.11.10.6:8080//directory/youpi.bla
curl -v http://10.11.10.6:8080//directory/nop

Test GET http://10.11.10.6:8080// - PASS
Test POST http://10.11.10.6:8080// with a size of 0 - PASS
Test HEAD http://10.11.10.6:8080// - PASS
Test GET http://10.11.10.6:8080//directory - PASS
Test GET http://10.11.10.6:8080//directory/youpi.bad_extension - PASS

Test GET http://10.11.10.6:8080//directory/youpi.bla - PASS
Test GET http://10.11.10.6:8080//directory/nop - PASS
Test GET http://10.11.10.6:8080//directory/nop/ - PASS
Test GET Expected 404 on http://10.11.10.6:8080//directory/nop/other.pouac - PASS
Test GET Expected 404 on http://10.11.10.6:8080//directory/Yeah - PASS
Test GET http://10.11.10.6:8080//directory/Yeah/not_happy.bad_extension - PASS

Test POST http://192.168.1.7:8080//directory/youpi.bla with a size of 100000000


➜  web-server git:(main) ✗ echo "hello world" | env \
    REQUEST_METHOD=POST \
    CONTENT_TYPE=test/file \
    CONTENT_LENGTH=11 \
    SERVER_PORT=8080 \
    REDIRECT_STATUS=200 \
    GATEWAY_INTERFACE=CGI/1.1 \
    SERVER_PROTOCOL=HTTP/1.1 \
    PATH_INFO=/directory/youpi.bla \
    ./cgi_tester
Status: 200 OK
Content-Type: text/html; charset=utf-8

HELLO WORLD%                                                                                  ➜  web-server git:(main) ✗ 




Test GET http://192.168.1.7:8080//directory/Yeah/not_happy.bad_extension
content returned: not happy bad extension


Test POST http://192.168.1.7:8080//directory/youpi.bla with a size of 100000000

Test POST http://192.168.1.7:8080//directory/youpla.bla with a size of 100000000

Test POST http://192.168.1.7:8080//directory/youpi.bla with a size of 100000 with special headers

Test POST http://192.168.1.7:8080//post_body with a size of 0

Test POST http://192.168.1.7:8080//post_body with a size of 100

Test POST http://192.168.1.7:8080//post_body with a size of 200

Test POST http://192.168.1.7:8080//post_body with a size of 101

Test multiple workers(5) doing multiple times(15): GET on /

Test multiple workers(20) doing multiple times(5000): GET on /

Test multiple workers(128) doing multiple times(50): GET on /directory/nop

Test multiple workers(20) doing multiple times(5): Post on /directory/youpi.bla with size 100000000
FATAL ERROR ON LAST TEST: bad status code
