#!/usr/bin/env python3
import os, sys, time
sys.stdout.write("Content-Type: text/plain\r\n\r\n")
sys.stdout.flush()
os.close(1)      # close CGI stdout pipe immediately (server sees EOF)
time.sleep(8)    # child still alive
