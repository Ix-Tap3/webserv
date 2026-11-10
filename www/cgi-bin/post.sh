#!/bin/bash
body=$(head -c "$CONTENT_LENGTH")
printf "Content-Type: text/plain\r\n\r\n"
echo "Recu $CONTENT_LENGTH octets ($CONTENT_TYPE) :"
echo "$body"
