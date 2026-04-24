#!/usr/bin/env python3

import os

print("Content-Type: text/html\r\n\r\n")

print("<html><body>")
print("<h1>CGI Test</h1>")

print("<h2>Environment Variables:</h2>")
for key, value in os.environ.items():
    print(f"<p>{key} = {value}</p>")

print("</body></html>")