#!/usr/bin/env python3
import os
import sys

data = sys.stdin.buffer.read()
sys.stdout.write("Content-Type: text/plain\r\n\r\n")
sys.stdout.write("Recu %d octets (CONTENT_LENGTH=%s, CONTENT_TYPE=%s) :\n"
                 % (len(data), os.environ.get("CONTENT_LENGTH", ""), os.environ.get("CONTENT_TYPE", "")))
sys.stdout.flush()
sys.stdout.buffer.write(data)
