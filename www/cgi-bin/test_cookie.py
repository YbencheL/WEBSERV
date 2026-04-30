#!/usr/bin/python3
import sys

# CGI scripts MUST print their own headers followed by a blank line
print("Content-Type: text/html")
print("Set-Cookie: user_id=ma1loc; Max-Age=3600; Path=/")
print("Set-Cookie: theme=dark; Path=/")
print("Custom-Header: Webserv-Test")
print("") # The empty line that separates headers from body

print("<html><body>")
print("<h1>CGI Cookie Test</h1>")
print("<p>If you see headers above this line, your parser isn't stripping them!</p>")
print("</body></html>")