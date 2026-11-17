#!/usr/bin/env python3
import os
import html

print("Content-Type: text/html\r\n\r\n", end="")
print("<h1>Variables d'environnement CGI</h1>")
print("<table border='1'>")
for key in sorted(os.environ):
    print("<tr><td>%s</td><td>%s</td></tr>" % (html.escape(key), html.escape(os.environ[key])))
print("</table>")
