echo "Process name: $1"
echo "Output directory: $2"

PID=$(pidof $1)
if [ -z "${PID}" ]; then
    echo "Process not found"
    exit 1
fi

sysctl -w kernel.perf_event_paranoid=-1
sysctl -w kernel.kptr_restrict=0

OUT_PATH="$2"
TIMESTAMP=$(date +%s)
OUT_FILE="$OUT_PATH/$TIMESTAMP.perf.data"
perf record -F 999 -g --call-graph fp -o "$OUT_FILE" -p $PID

echo "Data with callstack created, check $OUT_FILE"
