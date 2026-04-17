#!/usr/bin/env zsh
set -u

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BASE_URL="${1:-http://localhost:8080}"
CGI_BASE="${BASE_URL%/}/cgi-bin"
CGI_DIR="${ROOT_DIR}/www/cgi-bin"

HP_FILE="${ROOT_DIR}/multiplexer/handle_pipe_read.cpp"
CGI_FILE="${ROOT_DIR}/cgi/cgi.cpp"
TIMEOUT_FILE="${ROOT_DIR}/socket_init/init_socket_engine.cpp"

OK_SCRIPT="_ok_verify_not100.py"
SLOW_SCRIPT="_slow_verify_not100.py"
OK_PATH="${CGI_DIR}/${OK_SCRIPT}"
SLOW_PATH="${CGI_DIR}/${SLOW_SCRIPT}"

TMP_DIR="$(mktemp -d)"
trap 'rm -f "$OK_PATH" "$SLOW_PATH"; rm -rf "$TMP_DIR"' EXIT

fail_count=0
warn_count=0

pass() { echo "[PASS] $1"; }
fail() { echo "[FAIL] $1"; fail_count=$((fail_count + 1)); }
warn() { echo "[WARN] $1"; warn_count=$((warn_count + 1)); }

section() {
  echo
  echo "==== $1 ===="
}

section "Static checks for known timeout/CGI race risks"

if grep -q 'if (client.cgiHandler.state == CGI_READY)' "$HP_FILE"; then
  fail "handle_pipe_read is gated by CGI_READY (can block CGI_WAITING progress under load)."
else
  pass "No CGI_READY gate in handle_pipe_read."
fi

if grep -q 'checkResponseAndTime(epoll_fd, it->second);' "$TIMEOUT_FILE"; then
  if grep -q 'response_builder rb' "$TIMEOUT_FILE"; then
    pass "Timeout loop appears to build an error response when CGI becomes ERROR."
  else
    fail "Timeout loop checks CGI but does not build/send response on CGI ERROR."
  fi
else
  warn "Could not find CGI timeout check call in timeout loop (file may have changed)."
fi

if grep -q 'kill(pid, SIGTERM);' "$CGI_FILE" && grep -q 'wait = waitpid(pid, &status, WNOHANG);' "$CGI_FILE" && grep -q 'if (wait == pid)' "$CGI_FILE"; then
  fail "Immediate WNOHANG reap right after SIGTERM still present (state transition can be missed)."
else
  pass "No immediate-SIGTERM-reap fragile pattern detected."
fi

section "Runtime checks (done + timeout + mixed concurrency)"

cat > "$OK_PATH" <<'PY'
#!/usr/bin/env python3
print("Content-Type: text/plain")
print()
print("OK_VERIFY_NOT100")
PY

cat > "$SLOW_PATH" <<'PY'
#!/usr/bin/env python3
import signal
import time
signal.signal(signal.SIGTERM, signal.SIG_IGN)
print("Content-Type: text/plain")
print()
print("SLOW_START", flush=True)
time.sleep(30)
PY

chmod +x "$OK_PATH" "$SLOW_PATH"

ok_body="${TMP_DIR}/ok.body"
ok_code=$(curl -sS --max-time 10 -o "$ok_body" -w "%{http_code}" "${CGI_BASE}/${OK_SCRIPT}")
if [[ "$ok_code" == "200" ]] && grep -q 'OK_VERIFY_NOT100' "$ok_body"; then
  pass "Normal CGI done path returns 200."
else
  fail "Normal CGI done path failed (code=$ok_code)."
fi

timeout_body="${TMP_DIR}/to.body"
set +e
timeout_code=$(curl -sS --max-time 20 -o "$timeout_body" -w "%{http_code}" "${CGI_BASE}/${SLOW_SCRIPT}")
timeout_rc=$?
set -e
if [[ $timeout_rc -ne 0 ]]; then
  fail "Timeout CGI request hung/failed before server response (curl rc=$timeout_rc)."
elif [[ "$timeout_code" == "500" ]]; then
  pass "Timeout CGI returns HTTP 500."
else
  fail "Timeout CGI expected HTTP 500, got $timeout_code."
fi

# Mixed concurrency: if flow is fragile, this often surfaces as curl failures/timeouts.
concurrent_fail=0
for i in {1..6}; do
  (
    curl -sS --max-time 15 -o /dev/null "${CGI_BASE}/${OK_SCRIPT}" >/dev/null 2>&1
  ) &
  (
    curl -sS --max-time 20 -o /dev/null "${CGI_BASE}/${SLOW_SCRIPT}" >/dev/null 2>&1
  ) &
done
wait || concurrent_fail=1

if [[ $concurrent_fail -eq 0 ]]; then
  pass "Mixed concurrency run completed without client-side hangs."
else
  fail "Mixed concurrency run had at least one hung/failed request."
fi

section "Result"
if [[ $fail_count -eq 0 ]]; then
  echo "System looks healthy in this verification run."
  if [[ $warn_count -gt 0 ]]; then
    echo "Warnings: $warn_count"
  fi
  exit 0
else
  echo "NOT 100%: found $fail_count failure(s)."
  if [[ $warn_count -gt 0 ]]; then
    echo "Warnings: $warn_count"
  fi
  exit 1
fi
