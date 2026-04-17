#!/usr/bin/env python3
import signal
import time
signal.signal(signal.SIGTERM, signal.SIG_IGN)
print("Content-Type: text/plain")
print()
print("SLOW_START", flush=True)
time.sleep(30)
