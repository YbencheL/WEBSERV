#!/bin/bash
# Mini stress test: 2 workers × 2 iterations × 100MB POST to /directory/youpi.bla
# Mirrors the tester logic: all workers start at the same time, each does N iterations sequentially.
# Usage: ./mini_test.sh [workers=2] [iterations=2] [size=100000000]

# kernel OOM behavior

WORKERS=${1:-2}
ITERS=${2:-2}
SIZE=${3:-100000000}
HOST="http://localhost:8080"
URL="$HOST/directory/youpi.bla"

echo "======================================================"
echo " Mini CGI POST stress test"
echo " Workers   : $WORKERS"
echo " Iterations: $ITERS"
echo " Body size : $SIZE bytes"
echo " Target    : $URL"
echo "======================================================"

# Generate the body file once (reused by all workers) - EXACTLY SIZE bytes
BODY_FILE=$(mktemp /tmp/cgi_body_XXXXXX)
python3 -c "import sys; sys.stdout.buffer.write(b'X' * $SIZE)" > "$BODY_FILE"
ACTUAL_SIZE=$(stat -c%s "$BODY_FILE")
echo "Body file  : $BODY_FILE ($ACTUAL_SIZE bytes)"
echo ""

PASS=0
FAIL=0
RESULTS_DIR=$(mktemp -d /tmp/cgi_results_XXXXXX)

# Worker function: does ITERS sequential POSTs, writes result per iter to a file
worker() {
    local wid=$1
    for i in $(seq 1 $ITERS); do
        local out_file="$RESULTS_DIR/w${wid}_i${i}"
        local start_t=$SECONDS
        # Use curl with --data-binary @file, measure time, capture status line only
        HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" \
            -X POST \
            --data-binary "@$BODY_FILE" \
            -H "Content-Type: application/octet-stream" \
            --max-time 300 \
            "$URL")
        local elapsed=$(( SECONDS - start_t ))
        echo "${HTTP_CODE} ${elapsed}s" > "$out_file"
        echo "[worker $wid] iter $i → HTTP $HTTP_CODE (${elapsed}s)"
    done
}

# Start all workers in parallel
echo "Starting $WORKERS workers in parallel..."
echo ""
START_ALL=$SECONDS
for w in $(seq 1 $WORKERS); do
    worker "$w" &
done

# Wait for all workers
wait
TOTAL_TIME=$(( SECONDS - START_ALL ))

echo ""
echo "======================================================"
echo " Results"
echo "======================================================"

for f in "$RESULTS_DIR"/*; do
    code=$(awk '{print $1}' "$f")
    name=$(basename "$f")
    if [ "$code" = "200" ]; then
        PASS=$(( PASS + 1 ))
        echo "  ✅  $name → $code"
    else
        FAIL=$(( FAIL + 1 ))
        echo "  ❌  $name → $code"
    fi
done

TOTAL=$(( PASS + FAIL ))
echo ""
echo "  PASS: $PASS / $TOTAL"
echo "  FAIL: $FAIL / $TOTAL"
echo "  Total time: ${TOTAL_TIME}s"
echo "======================================================"

# Memory snapshot of the server process (if running)
SERVER_PID=$(pgrep -f "webserv" | head -1)
if [ -n "$SERVER_PID" ]; then
    RSS=$(awk '/VmRSS/ {print $2, $3}' /proc/$SERVER_PID/status 2>/dev/null)
    PEAK=$(awk '/VmPeak/ {print $2, $3}' /proc/$SERVER_PID/status 2>/dev/null)
    echo ""
    echo " Server PID     : $SERVER_PID"
    echo " Server RSS     : $RSS"
    echo " Server VmPeak  : $PEAK"
    echo "======================================================"
fi

# Cleanup
rm -f "$BODY_FILE"
rm -rf "$RESULTS_DIR"

if [ $FAIL -gt 0 ]; then
    exit 1
fi
exit 0
