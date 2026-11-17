#!/usr/bin/env python3
import os

print("Content-Type: text/html\r\n\r\n", end="")
print("<h1>Hello depuis un CGI Python</h1>")
print("<p>Methode : %s</p>" % os.environ.get("REQUEST_METHOD", ""))
print("<p>Query string : %s</p>" % os.environ.get("QUERY_STRING", ""))
print("<p>Dossier courant : %s</p>" % os.getcwd())
