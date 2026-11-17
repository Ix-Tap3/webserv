#!/usr/bin/env python3
import sys

size = 10 * 1024 * 1024
sys.stdout.write("Content-Type: text/plain\r\n\r\n")
chunk = "A" * 1023 + "\n"
for i in range(size // len(chunk)):
    sys.stdout.write(chunk)
