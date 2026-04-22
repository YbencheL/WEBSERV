#!/usr/bin/env zsh
set -euo pipefail

BASE_URL="${1:-http://localhost:8080}"
CGI_BASE="${BASE_URL%/}/cgi-bin"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
CGI_DIR="${ROOT_DIR}/www/cgi-bin"

OK_SCRIPT="_ok_cgi_test.py"
TIMEOUT_SCRIPT="_timeout_cgi_test.py"
OK_PATH="${CGI_DIR}/${OK_SCRIPT}"
TIMEOUT_PATH="${CGI_DIR}/${TIMEOUT_SCRIPT}"

TMP_DIR="$(mktemp -d)"
OK_HDR="${TMP_DIR}/ok.hdr"
OK_BODY="${TMP_DIR}/ok.body"
TO_HDR="${TMP_DIR}/timeout.hdr"
TO_BODY="${TMP_DIR}/timeout.body"

cleanup() {
  rm -f "$OK_PATH" "$TIMEOUT_PATH"
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

cat > "$OK_PATH" <<'PY'
#!/usr/bin/env python3
print("Content-Type: text/plain")
print()
print("OK_CGI_DONE")
PY

cat > "$TIMEOUT_PATH" <<'PY'
#!/usr/bin/env python3
import signal
import time

signal.signal(signal.SIGTERM, signal.SIG_IGN)

print("Content-Type: text/plain")
print()
print("START_TIMEOUT_TEST", flush=True)

time.sleep(30)
print("UNEXPECTED_END")
PY

chmod +x "$OK_PATH" "$TIMEOUT_PATH"

echo "[1/2] Testing normal CGI done path: ${CGI_BASE}/${OK_SCRIPT}"
ok_code=$(curl -sS -D "$OK_HDR" -o "$OK_BODY" -w "%{http_code}" "${CGI_BASE}/${OK_SCRIPT}")
if [[ "$ok_code" != "200" ]]; then
  echo "[FAIL] Expected HTTP 200 for done CGI, got: $ok_code"
  echo "----- response headers -----"
  cat "$OK_HDR"
  echo "----- response body -----"
  cat "$OK_BODY"
  exit 1
fi
if ! grep -q "OK_CGI_DONE" "$OK_BODY"; then
  echo "[FAIL] Done CGI response body mismatch (missing OK_CGI_DONE)"
  echo "----- response body -----"
  cat "$OK_BODY"
  exit 1
fi
echo "[PASS] Done CGI returned HTTP 200 with expected body"

echo "[2/2] Testing timeout CGI error path: ${CGI_BASE}/${TIMEOUT_SCRIPT}"
start_ts=$(date +%s)
set +e
to_code=$(curl -sS --max-time 20 -D "$TO_HDR" -o "$TO_BODY" -w "%{http_code}" "${CGI_BASE}/${TIMEOUT_SCRIPT}")
curl_rc=$?
set -e
end_ts=$(date +%s)
elapsed=$((end_ts - start_ts))

if [[ $curl_rc -ne 0 ]]; then
  echo "[FAIL] curl failed before timeout response (rc=$curl_rc)."
  echo "This usually means the server hung or did not send an error response in time."
  echo "Elapsed: ${elapsed}s"
  echo "----- response headers (if any) -----"
  [[ -f "$TO_HDR" ]] && cat "$TO_HDR"
  echo "----- response body (if any) -----"
  [[ -f "$TO_BODY" ]] && cat "$TO_BODY"
  exit 1
fi

if [[ "$to_code" != "500" ]]; then
  echo "[FAIL] Expected HTTP 500 for timeout CGI, got: $to_code"
  echo "Elapsed: ${elapsed}s"
  echo "----- response headers -----"
  cat "$TO_HDR"
  echo "----- response body -----"
  cat "$TO_BODY"
  exit 1
fi

if [[ $elapsed -gt 20 ]]; then
  echo "[FAIL] Timeout response took too long (${elapsed}s), expected <= 20s"
  exit 1
fi

echo "[PASS] Timeout CGI returned HTTP 500 in ${elapsed}s"
echo
echo "All checks passed."
