#!/usr/bin/env python3
import os
from http.cookies import SimpleCookie

cookies = SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
count = 0
if "visits" in cookies:
    try:
        count = int(cookies["visits"].value)
    except ValueError:
        count = 0
count += 1

print("Content-Type: text/html\r")
print("Set-Cookie: visits=%d; Path=/\r" % count)
print("\r")
print("<h1>Cookies Python</h1>")
print("<p>Nombre de visites : %d</p>" % count)
