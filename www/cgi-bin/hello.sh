#!/bin/bash
printf "Content-Type: text/html\r\n\r\n"
echo "<h1>Hello depuis un CGI bash</h1>"
echo "<p>Methode : $REQUEST_METHOD</p>"
echo "<p>Query string : $QUERY_STRING</p>"
echo "<p>Dossier courant : $(pwd)</p>"
