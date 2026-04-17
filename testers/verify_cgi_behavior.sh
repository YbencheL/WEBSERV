#!/usr/bin/env zsh
set -u

BASE_URL="${1:-http://localhost:8080}"
CGI_BASE="${BASE_URL%/}/cgi-bin"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
CGI_DIR="${ROOT_DIR}/www/cgi-bin"

TMP="$(mktemp -d)"
PASS=0
FAIL=0

cleanup() { rm -f "${CGI_DIR}/_t_"*.py; rm -rf "$TMP"; }
trap cleanup EXIT

pass() { echo "[PASS] $1"; PASS=$((PASS + 1)); }
fail() { echo "[FAIL] $1"; FAIL=$((FAIL + 1)); }
section() { echo; echo "==== $1 ===="; }

# -----------------------------------------------------------------------
# Helper: write a CGI python script
# -----------------------------------------------------------------------
mk_cgi() {
    local name="$1"
    local body="$2"
    local path="${CGI_DIR}/${name}"
    printf '#!/usr/bin/env python3\n%s\n' "$body" > "$path"
    /bin/chmod +x "$path"
}

# -----------------------------------------------------------------------
# TEST 1 — Normal CGI: server must return 200 with correct body
# -----------------------------------------------------------------------
section "TEST 1: Normal CGI done path"

mk_cgi "_t_ok.py" \
'print("Content-Type: text/plain\n")
print("RESULT_OK")' > /dev/null

code=$(curl -sS --max-time 10 -o "${TMP}/t1.body" -w "%{http_code}" "${CGI_BASE}/_t_ok.py")
if [[ "$code" == "200" ]] && grep -q "RESULT_OK" "${TMP}/t1.body"; then
    pass "Normal CGI: 200 + correct body"
else
    fail "Normal CGI: expected 200+body, got code=$code body=$(cat ${TMP}/t1.body 2>/dev/null)"
fi

# -----------------------------------------------------------------------
# TEST 2 — Timeout CGI: server must return 500, not hang
# Bug exposed: if check_all_client_timeouts() doesn't build error response
# after timeout kills the child, curl hangs forever.
# -----------------------------------------------------------------------
section "TEST 2: Timeout CGI — server must reply 500, not hang"

mk_cgi "_t_hang.py" \
'import signal, time
signal.signal(signal.SIGTERM, signal.SIG_IGN)
print("Content-Type: text/plain\n")
print("HANG_START", flush=True)
time.sleep(60)' > /dev/null

start=$(date +%s)
code=$(curl -sS --max-time 30 -o "${TMP}/t2.body" -w "%{http_code}" "${CGI_BASE}/_t_hang.py")
curl_rc=$?
elapsed=$(( $(date +%s) - start ))

if [[ $curl_rc -ne 0 ]]; then
    fail "Timeout CGI: curl failed or server hung for ${elapsed}s (rc=$curl_rc). Server never sent error response."
elif [[ "$code" == "500" ]]; then
    pass "Timeout CGI: got 500 in ${elapsed}s"
else
    fail "Timeout CGI: expected 500, got code=$code in ${elapsed}s"
fi

# -----------------------------------------------------------------------
# TEST 3 — After timeout, server must still handle new requests
# Bug exposed: if pipeOut[0] was closed but never removed from epoll,
# the next event loop iteration crashes or loops forever.
# -----------------------------------------------------------------------
section "TEST 3: Server still alive and serving after a timeout"

code=$(curl -sS --max-time 10 -o "${TMP}/t3.body" -w "%{http_code}" "${CGI_BASE}/_t_ok.py")
if [[ "$code" == "200" ]] && grep -q "RESULT_OK" "${TMP}/t3.body"; then
    pass "Server still serves 200 after timeout CGI"
else
    fail "Server broken after timeout CGI: code=$code body=$(cat ${TMP}/t3.body 2>/dev/null)"
fi

# -----------------------------------------------------------------------
# TEST 4 — CGI_READY gate race: send many normal requests concurrently.
# Bug exposed: if handle_pipe_read is gated on CGI_READY, some requests
# will get stuck in CGI_WAITING forever and curl will timeout.
# -----------------------------------------------------------------------
section "TEST 4: Concurrent normal CGI (race on CGI_READY gate)"

mk_cgi "_t_fast.py" \
'import os, sys
body = sys.stdin.read() if os.environ.get("CONTENT_LENGTH") else ""
print("Content-Type: text/plain\n")
print("FAST_OK")' > /dev/null

CONC=10
fail_conc=0
pids=()
for i in $(seq 1 $CONC); do
    curl -sS --max-time 10 \
         -o "${TMP}/conc_${i}.body" \
         -w "%{http_code}" \
         "${CGI_BASE}/_t_fast.py" > "${TMP}/conc_${i}.code" 2>/dev/null &
    pids+=($!)
done

for p in $pids; do wait $p 2>/dev/null || true; done

for i in $(seq 1 $CONC); do
    c=$(cat "${TMP}/conc_${i}.code" 2>/dev/null || echo "000")
    b=$(cat "${TMP}/conc_${i}.body" 2>/dev/null || echo "")
    if [[ "$c" != "200" ]] || ! echo "$b" | grep -q "FAST_OK"; then
        fail_conc=$((fail_conc + 1))
    fi
done

if [[ $fail_conc -eq 0 ]]; then
    pass "Concurrent CGI: all $CONC requests returned 200"
else
    fail "Concurrent CGI: $fail_conc/$CONC requests failed or hung (CGI_READY gate race)"
fi

# -----------------------------------------------------------------------
# TEST 5 — Mixed concurrent: normal + timeout CGIs at the same time
# Bug exposed: a blocking waitpid in reading() or a bad state gate will
# stall the event loop, causing the normal requests to also timeout.
# -----------------------------------------------------------------------
section "TEST 5: Mixed concurrent — normal + timeout CGIs simultaneously"

normal_pids=()
timeout_pids=()

for i in $(seq 1 5); do
    curl -sS --max-time 15 \
         -o "${TMP}/mix_ok_${i}.body" \
         -w "%{http_code}" \
         "${CGI_BASE}/_t_fast.py" > "${TMP}/mix_ok_${i}.code" 2>/dev/null &
    normal_pids+=($!)
done

for i in $(seq 1 3); do
    curl -sS --max-time 25 \
         -o "${TMP}/mix_to_${i}.body" \
         -w "%{http_code}" \
         "${CGI_BASE}/_t_hang.py" > "${TMP}/mix_to_${i}.code" 2>/dev/null &
    timeout_pids+=($!)
done

for p in $normal_pids $timeout_pids; do wait $p 2>/dev/null || true; done

norm_fail=0
for i in $(seq 1 5); do
    c=$(cat "${TMP}/mix_ok_${i}.code" 2>/dev/null || echo "000")
    b=$(cat "${TMP}/mix_ok_${i}.body" 2>/dev/null || echo "")
    if [[ "$c" != "200" ]] || ! echo "$b" | grep -q "FAST_OK"; then
        norm_fail=$((norm_fail + 1))
    fi
done

to_fail=0
for i in $(seq 1 3); do
    c=$(cat "${TMP}/mix_to_${i}.code" 2>/dev/null || echo "000")
    if [[ "$c" != "500" ]]; then
        to_fail=$((to_fail + 1))
    fi
done

[[ $norm_fail -eq 0 ]] && pass "Mixed concurrent: all 5 normal requests returned 200" \
                        || fail "Mixed concurrent: $norm_fail/5 normal requests failed (event loop stall)"
[[ $to_fail -eq 0 ]]   && pass "Mixed concurrent: all 3 timeout requests returned 500" \
                        || fail "Mixed concurrent: $to_fail/3 timeout requests did not return 500"

# -----------------------------------------------------------------------
# SUMMARY
# -----------------------------------------------------------------------
section "SUMMARY"
echo "PASS: $PASS   FAIL: $FAIL"
[[ $FAIL -eq 0 ]] && echo "All behavioral tests passed." || echo "NOT 100% — fix the FAILed tests above."
[[ $FAIL -eq 0 ]] && exit 0 || exit 1
