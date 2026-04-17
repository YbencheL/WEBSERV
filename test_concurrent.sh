#!/bin/bash
# simulate_last_test.sh
# Simulates: 20 workers × 5 times POST /directory/youpi.bla with 100MB

URL="http://192.168.1.7:8080/directory/youpi.bla"
WORKERS=20
ROUNDS=5
BODY_SIZE=100000000
PASS=0
FAIL=0
TOTAL=0

# Generate 100MB body once
dd if=/dev/urandom of=/tmp/body_100mb bs=100000 count=1000 2>/dev/null
run_worker() {
    local round=$1
    local worker=$2
    local status=$(curl -s -o /dev/null -w "%{http_code}" \
        -X POST \
        --data-binary @/tmp/body_100mb \
        -H "Content-Type: application/octet-stream" \
        --max-time 120 \
        "$URL")
    echo "Round $round Worker $worker -> HTTP $status"
    if [ "$status" != "200" ]; then
        echo "FAIL: Round $round Worker $worker got $status"
        exit 1
    fi
}

for round in $(seq 1 $ROUNDS); do
    echo "=== Round $round/$ROUNDS: launching $WORKERS concurrent workers ==="
    pids=()
    for worker in $(seq 1 $WORKERS); do
        run_worker $round $worker &
        pids+=($!)
    done
    # Wait for all workers in this round
    failed=0
    for pid in "${pids[@]}"; do
        if ! wait $pid; then
            failed=1
        fi
    done
    if [ $failed -ne 0 ]; then
        echo "=== ROUND $round FAILED ==="
    else
        echo "=== ROUND $round PASSED ==="
    fi
done

rm -f /tmp/body_100mb
