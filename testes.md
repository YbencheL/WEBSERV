# NON-BLOCKING test
# run it -> terminal A
curl -sS -o /dev/null http://localhost:8080/cgi-bin/eof_sleep.py
# run it -> terminal B
curl -sS -o /dev/null -w "GET / latency: %{time_total}s\n" http://localhost:8080/
