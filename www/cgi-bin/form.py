#!/usr/bin/env python3
import os
import sys
import html
from urllib.parse import parse_qs

method = os.environ.get("REQUEST_METHOD", "GET")
if method == "POST":
    length = int(os.environ.get("CONTENT_LENGTH", "0") or "0")
    raw = sys.stdin.read(length)
else:
    raw = os.environ.get("QUERY_STRING", "")

params = parse_qs(raw)
name = params.get("name", [""])[0]

print("Content-Type: text/html\r\n\r\n", end="")
print("<h1>Formulaire Python (%s)</h1>" % method)
if name:
    print("<p>Bonjour <b>%s</b> !</p>" % html.escape(name))
print("""
<form method="GET" action="/cgi-bin/form.py">
  <input name="name" placeholder="nom (GET)"> <button>Envoyer en GET</button>
</form>
<form method="POST" action="/cgi-bin/form.py">
  <input name="name" placeholder="nom (POST)"> <button>Envoyer en POST</button>
</form>
""")
