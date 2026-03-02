# 1. Find the PID of your program
TARGET_PID=$(pgrep -f signal-logic-do-not-do)

# 2. Check if we found it
if [ -z "$TARGET_PID" ]; then
    echo "Error: Program 'signal-logic-do-not-do' is not running!"
    exit 1
fi

# 3. Hammer it with SIGINT as fast as the shell allows
echo "Hammering PID $TARGET_PID with SIGINT... (Ctrl+C to stop script)"
while true; do 
    kill -INT "$TARGET_PID" 2>/dev/null || break
done

echo "Target process has exited or crashed."
